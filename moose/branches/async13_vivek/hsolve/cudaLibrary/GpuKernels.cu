/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <cstdio>
#include "GpuKernels.h"
#include "../HSolveStruct.h"	// For CompartmentStruct, etc.
#include "../HinesMatrix.h"		// For JunctionStruct
#include <cuda.h>
#include <cuda_runtime.h>

/*
 * Check CUDA return value and handle appropriately
 */
#define _(value) {															\
	{																		\
		cudaError_t _m_cudaStat = value;									\
		if (_m_cudaStat != cudaSuccess) {									\
			std::cerr << "Error " << cudaGetErrorString(_m_cudaStat)		\
					  << " at line " << __LINE__ << " in file "				\
					  << __FILE__ << std::endl;								\
			exit(1);														\
		}																	\
	}																		\
}

// Constants needed for active channels
// Note: cuda constant memory is implicitly static. Better to make it
// explicitly known here.
__constant__ static int INSTANT_X;
__constant__ static int INSTANT_Y;
__constant__ static int INSTANT_Z;

void setInstantXYZ(int x, int y, int z)
{
	_( cudaMemcpyToSymbol( INSTANT_X, &x, sizeof(int) ) );
	_( cudaMemcpyToSymbol( INSTANT_Y, &y, sizeof(int) ) );
	_( cudaMemcpyToSymbol( INSTANT_Z, &z, sizeof(int) ) );
}

__global__ void updateMatrixKernel(GpuDataStruct ds) {
	/*
	 * Copy contents of HJCopy_ into HJ_. Cannot do a vector assign() because
	 * iterators to HJ_ get invalidated in MS VC++
	 */
	if ( ds.HJSize != 0 )
		memcpy( ds.HJ, ds.HJCopy, sizeof( double ) * ds.HJSize );

	double *ihs = ds.HS;
	double *iv  = ds.V;
	
	//printf("In gpu: %lf %lf\n", ihs[0], ihs[3]);

	CompartmentStruct *ic;
	for ( ic = ds.compartment ; ic < ds.compartment + ds.nCompts ; ++ic ) {
		*ihs         = *( 2 + ihs );
		*( 3 + ihs ) = *iv * ic->CmByDt + ic->EmByRm;
		ihs += 4, ++iv;
	}
	
	// Not going to consider inject in GPU implementation at all.
	// Compartments with inject members can be declared as HSolve instead of
	// HSolveCuda.
}

__global__ void forwardEliminateKernel(GpuDataStruct ds) {
	unsigned int ic = 0;
	double *ihs = ds.HS;
	double **iop = ds.operand;
	JunctionStruct *junction;
	
	if ( iop ) {
		for( int x = 0 ; x < 36 ; x++ ) {		//XXX debugging only
			//printf( "%p ", *(iop + x) );
		}
		//printf("\n");
	}
	
	double pivot;
	double division;
	unsigned int index;
	unsigned int rank;
	double *j, *s;
	for ( junction = ds.junction;
	      junction < ds.junction + ds.junctionSize;
	      junction++ )
	{
		index = junction->index;
		rank = junction->rank;
		
		while ( ic < index ) {
			*( ihs + 4 ) -= *( ihs + 1 ) / *ihs * *( ihs + 1 );
			*( ihs + 7 ) -= *( ihs + 1 ) / *ihs * *( ihs + 3 );
			
			++ic, ihs += 4;
		}
		
		pivot = *ihs;
		if ( rank == 1 ) {
			//printf("rank=1; ");
			//printf("ic: %d ", ic);
			//printf("ihs: %p ", ihs);
			//printf("iop: %p ", iop);
			j = *iop;
			s = *(iop + 1);
			
			//printf( "s: %p\n", s );
			
			division    = *( j + 1 ) / pivot;
			*( s )     -= division * *j;
			*( s + 3 ) -= division * *( ihs + 3 );
			
			iop += 3;
		} else if ( rank == 2 ) {
			//printf("rank=2; ");
			//printf("ic: %d ", ic);
			//printf("ihs: %p ", ihs);
			//printf("iop: %p ", iop);
			j = *iop;
			
			s           = *( iop + 1 );
			//printf( "s: %p ", s );
			division    = *( j + 1 ) / pivot;
			*( s )     -= division * *j;
			*( j + 4 ) -= division * *( j + 2 );
			*( s + 3 ) -= division * *( ihs + 3 );
			
			s           = *( iop + 3 );
			//printf( "s: %p\n", s );
			division    = *( j + 3 ) / pivot;
			*( j + 5 ) -= division * *j;
			*( s )     -= division * *( j + 2 );
			*( s + 3 ) -= division * *( ihs + 3 );
			
			iop += 5;
		} else {
			//printf("rank=%d; ", rank);
			//printf("ic: %d ", ic);
			//printf("ihs: %p ", ihs);
			//printf("iop: %p\n", iop);
			double **end = iop + 3 * rank * ( rank + 1 );
			for ( ; iop < end; iop += 3 )
				**iop -= **( iop + 2 ) / pivot * **( iop + 1 );
		}
		
		++ic, ihs += 4;
	}
	
	while ( ic < ds.nCompts - 1 ) {
		*( ihs + 4 ) -= *( ihs + 1 ) / *ihs * *( ihs + 1 );
		*( ihs + 7 ) -= *( ihs + 1 ) / *ihs * *( ihs + 3 );
		
		++ic, ihs += 4;
	}
}

__global__ void backwardSubstituteKernel(GpuDataStruct ds) {
	// We are reverse iterating here, so all pointers are initialized to the
	// ultimate elements of their respective arrays.
	int 	ic		 = ds.nCompts - 1;
	double *ivmid	 = ds.VMid + ic;
	double *iv		 = ds.V + ic;
	double *ihs		 = ds.HS + 4 * ds.nCompts - 1;
	double **iop	 = ds.operand + ds.operandSize - 1;
	double **ibop	 = ds.backOperand + ds.backOperandSize - 1;
	JunctionStruct *junction = ds.junction + ds.junctionSize - 1;
	
	//printf("In gpu: %lf %lf\n", ihs[-3], ihs[0]);

	*ivmid = *ihs / *( ihs - 3 );
	*iv = 2 * *ivmid - *iv;
	--ic, --ivmid, --iv, ihs -= 4;
	
	int index;
	int rank;
	for ( ;
	      ds.junction != NULL && junction >= ds.junction;
	      junction-- )
	{
		index = junction->index;
		rank = junction->rank;
		
		while ( ic > index ) {
			// ivmid was -1, so now it's +1!
			*ivmid = ( *ihs - *( ihs - 2 ) * *( ivmid + 1 ) ) / *( ihs - 3 );
			*iv = 2 * *ivmid - *iv;
			
			--ic, --ivmid, --iv, ihs -= 4;
		}
		
		if ( rank == 1 ) {
			*ivmid = ( *ihs - **iop * **( iop - 2 ) ) / *( ihs - 3 );
			
			iop -= 3;
		} else if ( rank == 2 ) {
			double *v0 = *( iop );
			double *v1 = *( iop - 2 );
			double *j  = *( iop - 4 );
			
			*ivmid = ( *ihs
			           - *v0 * *( j + 2 )	// j was a vdIterator in forward!
			           - *v1 * *j			// so + remains +!!
			         ) / *( ihs - 3 );
			
			iop -= 5;
		} else {
			*ivmid = *ihs;
			for ( int i = 0; i < rank; ++i ) {
				*ivmid -= **ibop * **( ibop - 1 );
				ibop -= 2;
			}
			*ivmid /= *( ihs - 3 );
			
			iop -= 3 * rank * ( rank + 1 );
		}
		
		*iv = 2 * *ivmid - *iv;
		--ic, --ivmid, --iv, ihs -= 4;
	}
	
	while ( ic >= 0 ) {
		// The ivmid was -1, so now it becomes +1!
		*ivmid = ( *ihs - *( ihs - 2 ) * *( ivmid + 1 ) ) / *( ihs - 3 );
		*iv = 2 * *ivmid - *iv;
		
		--ic, --ivmid, --iv, ihs -= 4;
	}

	//printf("V in gpu: %lf %lf\n", ds.VMid[0], ds.V[0]);
}

__device__ void findRow(GpuLookupTable table, double value, GpuLookupRow &row)
{
	if ( value < table.min )
		value = table.min;
	else if ( value > table.max )
		value = table.max;

	double div = ( value - table.min ) / table.dx;
	unsigned int integer = ( unsigned int )( div );

	row.fraction = div - integer;
	row.row = table.table + integer * table.nColumns;
}

__device__ void lookupTable(GpuLookupTable table, GpuLookupColumn column,
							GpuLookupRow row, double &C1, double &C2)
{
	double a, b;
	double *ap, *bp;

	ap = row.row + column.column;

	bp = ap + table.nColumns;

	a = *ap;
	b = *bp;
	C1 = a + ( b - a ) * row.fraction;

	a = *( ap + 1 );
	b = *( bp + 1 );
	C2 = a + ( b - a ) * row.fraction;
}

__global__ void advanceChannelsKernel(GpuDataStruct ds, double dt)
{
	double			 *iv;
	double			 *istate		 = ds.state;
	int				 *ichannelcount	 = ds.channelCount;
	ChannelStruct	 *ichan			 = ds.channel;
	ChannelStruct	 *chanBoundary;
	unsigned int	 *icacount		 = ds.caCount;
	double			 *ica			 = ds.ca;
	double			 *caBoundary;
	GpuLookupColumn	 *icolumn		 = ds.column;
	GpuLookupRow	 *icarowcompt;
	GpuLookupRow	 **icarow		 = ds.caRow;

	GpuLookupRow vRow;
	double C1, C2;
	for ( iv = ds.V ; iv != ds.V + ds.nCompts ; ++iv ) {
		findRow( ds.vTable, *iv, vRow );
		icarowcompt = ds.caRowCompt;
		caBoundary = ica + *icacount;
		for ( ; ica < caBoundary; ++ica, ++icarowcompt )
			// Implicitly assumes that we are not going out of bounds for
			// caRowCompt! => The size of caRowCompt must be the max number of
			// calcium pools out of all compartments.
			findRow( ds.caTable, *ica, *icarowcompt );

		/*
		 * Optimize by moving "if ( instant )" outside the loop, because it is
		 * rarely used. May also be able to avoid "if ( power )".
		 *
		 * Or not: excellent branch predictors these days.
		 *
		 * Will be nice to test these optimizations.
		 */
		chanBoundary = ichan + *ichannelcount;
		for ( ; ichan < chanBoundary ; ++ichan ) {
			if ( ichan->Xpower_ > 0.0 ) {
				lookupTable( ds.vTable, *icolumn, vRow, C1, C2 );
				if ( ichan->instant_ & INSTANT_X )
					*istate = C1 / C2;
				else {
					double temp = 1.0 + dt / 2.0 * C2;
					*istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
				}
				++icolumn, ++istate;
			}

			if ( ichan->Ypower_ > 0.0 ) {
				lookupTable( ds.vTable, *icolumn, vRow, C1, C2 );
				if ( ichan->instant_ & INSTANT_Y )
					*istate = C1 / C2;
				else {
					double temp = 1.0 + dt / 2.0 * C2;
					*istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
				}
				++icolumn, ++istate;
			}

			if ( ichan->Zpower_ > 0.0 ) {
				GpuLookupRow *caRow = *icarow;
				if ( caRow ) {
					lookupTable( ds.caTable, *icolumn, *caRow, C1, C2 );
				} else {
					lookupTable( ds.vTable, *icolumn, vRow, C1, C2 );
				}

				if ( ichan->instant_ & INSTANT_Z )
					*istate = C1 / C2;
				else {
					double temp = 1.0 + dt / 2.0 * C2;
					*istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
				}

				++icolumn, ++istate, ++icarow;
			}
		}

		++ichannelcount, ++icacount;
	}
}

__global__ void calculateChannelCurrentsKernel(GpuDataStruct ds)
{
	ChannelStruct *ichan;
	CurrentStruct *icurrent = ds.current;

	if ( ds.stateSize != 0 ) {
		double *istate = ds.state;

		for ( ichan = ds.channel ; ichan != ds.channel + ds.nChannels ;
			  ++ichan, ++icurrent )
		{
			// Stuff inside ichan->process
			double fraction = 1;
			// No complications for taking power
			if ( ichan->Xpower_ > 0.0 )
				fraction *= pow( *( istate++ ), ichan->Xpower_ );
			if ( ichan->Ypower_ > 0.0 )
				fraction *= pow( *( istate++ ), ichan->Ypower_ );
			if ( ichan->Zpower_ > 0.0 )
				fraction *= pow( *( istate++ ), ichan->Zpower_ );
			icurrent->Gk = ichan->Gbar_ * fraction;
		}
	}
}

__global__ void advanceCalciumKernel(GpuDataStruct ds)
{
	double **icatarget = ds.caTarget;
	double *ivmid = ds.VMid;
	CurrentStruct *icurrent = ds.current;
	CurrentStruct **iboundary = ds.currentBoundary;

	// Reset caActivation to zero
	memset( ds.caActivation, 0.0, ds.nCaPools );

	// caAdvance is just taken to be 1. The 0 case is not implemented.

	// There are as many current boundaries as there are compartments.
	for ( ; iboundary != ds.currentBoundary + ds.nCompts ; ++iboundary ) {
		for ( ; icurrent < *iboundary ; ++icurrent ) {
			if ( *icatarget )
				**icatarget += icurrent->Gk * ( icurrent->Ek - *ivmid );
			++icatarget;
		}
		++ivmid;
	}

	CaConcStruct *icaconc;
	double *icaactivation = ds.caActivation;
	double *ica = ds.ca;
	for( icaconc = ds.caConc; icaconc != ds.caConc + ds.nCaPools; ++icaconc ) {
		// CaConcStruct::process has been inserted here, fully expanded
		icaconc->c_ = 	icaconc->factor1_ * icaconc->c_
					  + icaconc->factor2_ * *icaactivation;
		double ca = icaconc->CaBasal_ + icaconc->c_;
		if ( icaconc->ceiling_ > 0 && ca > icaconc->ceiling_ ) {
			ca = icaconc->ceiling_;
			icaconc->c_ = ca - icaconc->CaBasal_;	// CaConcStruct::setCa
		}
		if ( ca < icaconc->floor_ ) {
			ca = icaconc->floor_;
			icaconc->c_ = ca - icaconc->CaBasal_;	// CaConcStruct::setCa
		}
		*ica = ca;
		++ica, ++icaactivation;
	}
}
