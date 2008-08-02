/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include <queue>
#include "SynInfo.h"
#include "RateLookup.h"
#include "HSolveStruct.h"
#include "HSolveBase.h"
#include <cmath>

#include "SpikeGen.h"

const int HSolveBase::INSTANT_X = 1;
const int HSolveBase::INSTANT_Y = 2;
const int HSolveBase::INSTANT_Z = 4;

void HSolveBase::step( ProcInfo info ) {
	if ( !Gk_.size() ) {
		Gk_.resize( channel_.size() );
		GkEk_.resize( channel_.size() );
	}

	//~ advanceChannels( );
	advanceChannels( info->dt_ );
	calculateChannelCurrents( );
	updateMatrix( );
	forwardEliminate( );
	backwardSubstitute( );
	advanceCalcium( );
	advanceSynChans( info );
	sendSpikes( info );
}

void HSolveBase::calculateChannelCurrents( ) {
	vector< ChannelStruct >::iterator ichan;
	vector< double >::iterator igk = Gk_.begin();
	vector< double >::iterator igkek = GkEk_.begin();
	double* istate = &state_[ 0 ];
	
	for ( ichan = channel_.begin(); ichan != channel_.end(); ++ichan ) {
		ichan->process( istate, *igk, *igkek );
		++igk, ++igkek;
	}
}

void HSolveBase::updateMatrix( ) {
	Mbranch_.assign( MbranchCopy_.begin(), MbranchCopy_.end() );
	
	double GkSum, GkEkSum;
	
	unsigned char ichan;
	vector< unsigned char >::iterator icco = channelCount_.begin();
	vector< double >::iterator iml     = Mlinear_.begin();
	vector< double >::iterator iv      = V_.begin();
	vector< double >::iterator icmbydt = CmByDt_.begin();
	vector< double >::iterator iembyrm = EmByRm_.begin();
	vector< double >::iterator iinject = inject_.begin();
	vector< double >::iterator igk     = Gk_.begin();
	vector< double >::iterator igkek   = GkEk_.begin();
	for ( unsigned long ic = 0; ic < N_; ++ic ) {
		GkSum   = 0.0;
		GkEkSum = 0.0;
		for ( ichan = 0; ichan < *icco; ++ichan ) {
			GkSum   += *( igk++ );
			GkEkSum += *( igkek++ );
		}
		
		*iml         = *( 2 + iml ) + GkSum;
		*( 3 + iml ) = *iembyrm + *icmbydt * *iv + GkEkSum + *iinject;
		//~ *ia         = *( 3 + ia ) + *icmbydt * GkSum;
		//~ *( 4 + ia ) = *iv + *icmbydt * (*iembyrm + GkEkSum + *iinject);
		++icco, iml += 4, ++icmbydt, ++iv, ++iembyrm, ++iinject;
	}
	
	unsigned int ic;
	vector< SynChanStruct >::iterator isyn;
	for ( isyn = synchan_.begin(); isyn != synchan_.end(); ++isyn ) {
		ic = isyn->compt_;
		Mlinear_[ 4 * ic ] += isyn->Gk_;
		Mlinear_[ 4 * ic + 3 ] += isyn->Gk_ * isyn->Ek_;
	}
}

void HSolveBase::forwardEliminate( ) {
	unsigned int ic = 0;
	vector< double >::iterator iml = Mlinear_.begin();
	vector< double* >::iterator iop = operand_.begin();
	vector< BranchStruct >::iterator branch;
	
	double *l;
	double *b;
	double pivot;
	unsigned int index;
	unsigned int rank;
	for ( branch = branch_.begin(); branch != branch_.end(); ++branch ) {
		index = branch->index;
		rank = branch->rank;
		
		while ( ic < index ) {
			*( iml + 4 ) -= *( iml + 1 ) / *iml * *( iml + 1 );
			*( iml + 7 ) -= *( iml + 1 ) / *iml * *( iml + 3 );
			
			++ic, iml += 4;
		}
		
		pivot = *iml;
		if ( rank == 1 ) {
			*( iml + 4 ) -= *( *iop + 1 ) / pivot * **iop;
			*( iml + 7 ) -= *( *iop + 1 ) / pivot * *( iml + 3 );
			
			iop += 1;
		} else if ( rank == 2 ) {
			l = *iop;
			b = *( iop + 1 );
			
			*l         -= *( b + 1 ) / pivot * *b;
			*( l + 3 ) -= *( b + 1 ) / pivot * *( iml + 3 );
			*( l + 4 ) -= *( b + 3 ) / pivot * *( b + 2 );
			*( l + 7 ) -= *( b + 3 ) / pivot * *( iml + 3 );
			*( b + 4 ) -= *( b + 1 ) / pivot * *( b + 2 );
			*( b + 5 ) -= *( b + 3 ) / pivot * *b;
			
			iop += 3;
		} else {
			vector< double* >::iterator
				end = iop + 3 * rank * ( rank + 1 );
			for ( ; iop < end; iop += 3 )
				**iop -= **( iop + 2 ) / pivot * **( iop + 1 );
		}
		
		++ic, iml += 4;
	}
	
	while ( ic < N_ ) {
		*( iml + 4 ) -= *( iml + 1 ) / *iml * *( iml + 1 );
		*( iml + 7 ) -= *( iml + 1 ) / *iml * *( iml + 3 );
		
		++ic, iml += 4;
	}
}

void HSolveBase::backwardSubstitute( ) {
	int ic = N_ - 1;
	vector< double >::reverse_iterator ivmid = VMid_.rbegin();
	vector< double >::reverse_iterator iv = V_.rbegin();
	vector< double >::reverse_iterator iml = Mlinear_.rbegin();
	vector< double* >::reverse_iterator iop = operand_.rbegin();
	vector< double* >::reverse_iterator ibop = backOperand_.rbegin();
	vector< BranchStruct >::reverse_iterator branch;
	
	*ivmid = *iml / *( iml + 3 );
	*iv = 2 * *ivmid - *iv;
	--ic, ++ivmid, ++iv, iml += 4;
	
	double *b;
	double *v;
	int index;
	int rank;
	for ( branch = branch_.rbegin(); branch != branch_.rend(); ++branch ) {
		index = branch->index;
		rank = branch->rank;
		
		while ( ic > index ) {
			*ivmid = ( *iml - *( iml + 2 ) * *( ivmid - 1 ) ) / *( iml + 3 );
			*iv = 2 * *ivmid - *iv;
			
			--ic, ++ivmid, ++iv, iml += 4;
		}
		
		if ( rank == 1 ) {
			*ivmid = ( *iml - **iop * *( ivmid - 1 ) ) / *( iml + 3 );
			
			iop += 1;
		} else if ( rank == 2 ) {
			v = *iop;
			b = *( iop + 1 );
			
			*ivmid = ( *iml
			           - *b * *v
			           - *( b + 2 ) * *( v + 1 )
			         ) / *( iml + 3 );
			
			iop += 3;
		} else {
			*ivmid = *iml;
			for ( int i = 0; i < rank; ++i ) {
				*ivmid -= **ibop * **( ibop + 1 );
				ibop += 2;
			}
			*ivmid /= *( iml + 3 );
			
			iop += 3 * rank * ( rank + 1 );
		}
		
		*iv = 2 * *ivmid - *iv;
		--ic, ++ivmid, ++iv, iml += 4;
	}
	
	while ( ic >= 0 ) {
		*ivmid = ( *iml - *( iml + 2 ) * *( ivmid - 1 ) ) / *( iml + 3 );
		*iv = 2 * *ivmid - *iv;
		
		--ic, ++ivmid, ++iv, iml += 4;
	}
}

void HSolveBase::advanceCalcium( ) {
	unsigned char ichan;
	vector< double* >::iterator icatarget = caTarget_.begin();
	vector< double >::iterator igk = Gk_.begin();
	vector< double >::iterator igkek = GkEk_.begin();
	vector< double >::iterator ivmid = VMid_.begin();
	vector< unsigned char >::iterator icco;
	
	caActivation_.assign( caActivation_.size(), 0.0 );
	
double v;
vector< double >::iterator iv = V_.begin();
	for ( icco = channelCount_.begin(); icco != channelCount_.end(); ++icco, ++ivmid,
	++iv )
		for ( ichan = 0; ichan < *icco; ++ichan, ++icatarget, ++igk, ++igkek )
		{
			v = 2 * *ivmid - *iv;
			if ( *icatarget )
				**icatarget += *igkek - *igk * v;
				//~ **icatarget += *igkek - *igk * *ivmid;
		}
	
	vector< CaConcStruct >::iterator icaconc;
	vector< double >::iterator icaactivation = caActivation_.begin();
	vector< double >::iterator ica = ca_.begin();
	for ( icaconc = caConc_.begin(); icaconc != caConc_.end(); ++icaconc )
	{
		*ica = icaconc->process( *icaactivation );
		++ica, ++icaactivation;
	}
}

void HSolveBase::advanceChannels( double dt ) {
	vector< double >::iterator iv;
	vector< double >::iterator istate = state_.begin();
	vector< double* >::iterator icadepend = caDepend_.begin();
	vector< RateLookup >::iterator ilookup = lookup_.begin();
	vector< unsigned char >::iterator icco = channelCount_.begin();
	
	LookupKey key;
	LookupKey keyCa;
	double C1, C2;
	vector< ChannelStruct >::iterator ichan = channel_.begin();
	vector< ChannelStruct >::iterator nextChan;
	for ( iv = V_.begin(); iv != V_.end(); ++iv, ++icco ) {
		if ( *icco == 0 )
			continue;
		
		ilookup->getKey( *iv, key );
		nextChan = ichan + *icco;
		for ( ; ichan < nextChan; ++ichan, ++icadepend ) {
			if ( ichan->Xpower_ ) {
				ilookup->rates( key, C1, C2 );
				//~ *istate = *istate * C1 + C2;
				//~ *istate = ( C1 + ( 2 - C2 ) * *istate ) / C2;
				if ( ichan->instant_ & INSTANT_X )
					*istate = C1 / C2;
				else {
					double temp = 1.0 + dt / 2.0 * C2;
					*istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
				}
				
				++ilookup, ++istate;
			}
			
			if ( ichan->Ypower_ ) {
				ilookup->rates( key, C1, C2 );
				//~ *istate = *istate * C1 + C2;
				//~ *istate = ( C1 + ( 2 - C2 ) * *istate ) / C2;
				if ( ichan->instant_ & INSTANT_Y )
					*istate = C1 / C2;
				else {
					double temp = 1.0 + dt / 2.0 * C2;
					*istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
				}
								
				++ilookup, ++istate;
			}
			
			if ( ichan->Zpower_ ) {
				if ( *icadepend ) {
					ilookup->getKey( **icadepend, keyCa );
					ilookup->rates( keyCa, C1, C2 );
				} else
					ilookup->rates( key, C1, C2 );
				
				//~ *istate = *istate * C1 + C2;
				//~ *istate = ( C1 + ( 2 - C2 ) * *istate ) / C2;
				if ( ichan->instant_ & INSTANT_Z )
					*istate = C1 / C2;
				else {
					double temp = 1.0 + dt / 2.0 * C2;
					*istate = ( *istate * ( 2.0 - temp ) + dt * C1 ) / temp;
				}
				
				++ilookup, ++istate;
			}
		}
	}
}

void HSolveBase::advanceSynChans( ProcInfo info ) {
	vector< SynChanStruct >::iterator isyn;
	for ( isyn = synchan_.begin(); isyn != synchan_.end(); ++isyn )
		isyn->process( info );
}

void HSolveBase::sendSpikes( ProcInfo info ) {
	vector< SpikeGenStruct >::iterator ispike;
	for ( ispike = spikegen_.begin(); ispike != spikegen_.end(); ++ispike ) {
		set< double >( ispike->elm_, "Vm", V_[ ispike->compt_ ] );
		SetConn c( ispike->elm_, 0 );
		SpikeGen::processFunc( &c, info );
	}
}
