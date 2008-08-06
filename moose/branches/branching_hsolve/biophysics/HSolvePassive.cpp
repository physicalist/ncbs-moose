/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include "SynInfo.h"		// for SynChanStruct in BioScan. Remove eventually.
#include <queue>			// for SynChanStruct in BioScan. Remove eventually.
#include "HSolveStruct.h"	// for SynChanStruct in BioScan. Remove eventually.
#include "BioScan.h"
#include "HinesMatrix.h"
#include "HSolvePassive.h"

void HSolvePassive::setup( Id seed, double dt ) {
	dt_ = dt;
	
	walkTree( seed );
	initialize( );
	storeTree( );
	
	HinesMatrix::setup( tree_, Ga_, CmByDt_ );
}

void HSolvePassive::solve( ) {
	forwardEliminate( );
	backwardSubstitute( );
}

//////////////////////////////////////////////////////////////////////
// Setup of data structures
//////////////////////////////////////////////////////////////////////

void HSolvePassive::walkTree( Id seed ) {
	// Find leaf node
	Id previous;
	vector< Id > recent;
	BioScan::adjacent( seed, recent );
	if ( recent.size() != 1 )
		while ( !recent.empty() ) {
			previous = seed;
			seed = recent[ 0 ];
			BioScan::adjacent( seed, previous, recent );
		}
	
	// Depth-first search
	vector< vector< Id > > cstack;
	Id above;
	Id current;
	cstack.resize( 1 );
	cstack[ 0 ].push_back( seed );
	while ( !cstack.empty() ) {
		vector< Id >& top = cstack.back();
		
		if ( top.empty() ) {
			cstack.pop_back();
			if ( !cstack.empty() )
				cstack.back().pop_back();
		} else {
			if ( cstack.size() > 1 )
				above = cstack[ cstack.size() - 2 ].back();
			
			current = top.back();
			compartmentId_.push_back( current );
			
			cstack.resize( cstack.size() + 1 );
			BioScan::adjacent( current, above, cstack.back() );
		}
	}
	
	// Compartments get ordered according to their hines' indices once this
	// list is reversed.
	reverse( compartmentId_.begin(), compartmentId_.end() );
}

void HSolvePassive::initialize( ) {
	double Vm, Cm, Em, Rm, Ra, inject;
	vector< Id >::iterator ic;
	for ( ic = compartmentId_.begin(); ic != compartmentId_.end(); ++ic ) {
		get< double >( ( *ic )(), "initVm", Vm );
		get< double >( ( *ic )(), "Cm", Cm );
		get< double >( ( *ic )(), "Em", Em );
		get< double >( ( *ic )(), "Rm", Rm );
		get< double >( ( *ic )(), "Ra", Ra );
		get< double >( ( *ic )(), "inject", inject );
		
		CompartmentStruct compartment (
			2.0 * Cm / dt_,
			Em / Rm,
			inject
		);
		
		V_.push_back( Vm );
		Ga_.push_back( 2.0 / Ra );
		compartment_.push_back( compartment );
	}
}

void HSolvePassive::storeTree( ) {
	map< Id, unsigned int > hinesIndex;
	for ( unsigned int ic = 0; ic < compartmentId_.size(); ++ic )
		hinesIndex[ compartmentId_[ ic ] ] = ic;
	
	vector< Id > children;
	vector< Id >::iterator child;
	
	for ( unsigned int ic = 0; ic < compartmentId_.size(); ++ic ) {
		const Id& parent = compartmentId_[ ic ];
		
		children.clear();
		BioScan::children( parent, children );
		
		tree_.resize( tree_.size() + 1 );
		// Push hines' index of parent
		tree_.back().push_back( ic );
		
		// Push hines' indices of children
		for ( child = children.begin(); child != children.end(); ++child )
			tree_.back().push_back(
				hinesIndex[ *child ] );
		
		CmByDt_.push_back( compartment_[ ic ].CmByDt );
	}
}

//////////////////////////////////////////////////////////////////////
// Numerical integration
//////////////////////////////////////////////////////////////////////

void HSolvePassive::forwardEliminate( ) {
	unsigned int ic = 0;
	vector< double >::iterator ihs = HS_.begin();
	vector< double* >::iterator iop = operand_.begin();
	vector< JunctionStruct >::iterator junction;
	
	double *l;
	double *b;
	double pivot;
	unsigned int index;
	unsigned int rank;
	for ( junction = junction_.begin(); junction != junction_.end(); ++junction ) {
		index = junction->index;
		rank = junction->rank;
		
		while ( ic < index ) {
			*( ihs + 4 ) -= *( ihs + 1 ) / *ihs * *( ihs + 1 );
			*( ihs + 7 ) -= *( ihs + 1 ) / *ihs * *( ihs + 3 );
			
			++ic, ihs += 4;
		}
		
		pivot = *ihs;
		if ( rank == 1 ) {
			*( ihs + 4 ) -= *( *iop + 1 ) / pivot * **iop;
			*( ihs + 7 ) -= *( *iop + 1 ) / pivot * *( ihs + 3 );
			
			iop += 1;
		} else if ( rank == 2 ) {
			l = *iop;
			b = *( iop + 1 );
			
			*l         -= *( b + 1 ) / pivot * *b;
			*( l + 3 ) -= *( b + 1 ) / pivot * *( ihs + 3 );
			*( l + 4 ) -= *( b + 3 ) / pivot * *( b + 2 );
			*( l + 7 ) -= *( b + 3 ) / pivot * *( ihs + 3 );
			*( b + 4 ) -= *( b + 1 ) / pivot * *( b + 2 );
			*( b + 5 ) -= *( b + 3 ) / pivot * *b;
			
			iop += 3;
		} else {
			vector< double* >::iterator
				end = iop + 3 * rank * ( rank + 1 );
			for ( ; iop < end; iop += 3 )
				**iop -= **( iop + 2 ) / pivot * **( iop + 1 );
		}
		
		++ic, ihs += 4;
	}
	
	while ( ic < nCompt_ ) {
		*( ihs + 4 ) -= *( ihs + 1 ) / *ihs * *( ihs + 1 );
		*( ihs + 7 ) -= *( ihs + 1 ) / *ihs * *( ihs + 3 );
		
		++ic, ihs += 4;
	}
}

void HSolvePassive::backwardSubstitute( ) {
	int ic = nCompt_ - 1;
	vector< double >::reverse_iterator ivmid = VMid_.rbegin();
	vector< double >::reverse_iterator iv = V_.rbegin();
	vector< double >::reverse_iterator ihs = HS_.rbegin();
	vector< double* >::reverse_iterator iop = operand_.rbegin();
	vector< double* >::reverse_iterator ibop = backOperand_.rbegin();
	vector< JunctionStruct >::reverse_iterator junction;
	
	*ivmid = *ihs / *( ihs + 3 );
	*iv = 2 * *ivmid - *iv;
	--ic, ++ivmid, ++iv, ihs += 4;
	
	double *b;
	double *v;
	int index;
	int rank;
	for ( junction = junction_.rbegin(); junction != junction_.rend(); ++junction ) {
		index = junction->index;
		rank = junction->rank;
		
		while ( ic > index ) {
			*ivmid = ( *ihs - *( ihs + 2 ) * *( ivmid - 1 ) ) / *( ihs + 3 );
			*iv = 2 * *ivmid - *iv;
			
			--ic, ++ivmid, ++iv, ihs += 4;
		}
		
		if ( rank == 1 ) {
			*ivmid = ( *ihs - **iop * *( ivmid - 1 ) ) / *( ihs + 3 );
			
			iop += 1;
		} else if ( rank == 2 ) {
			v = *iop;
			b = *( iop + 1 );
			
			*ivmid = ( *ihs
			           - *b * *v
			           - *( b + 2 ) * *( v + 1 )
			         ) / *( ihs + 3 );
			
			iop += 3;
		} else {
			*ivmid = *ihs;
			for ( int i = 0; i < rank; ++i ) {
				*ivmid -= **ibop * **( ibop + 1 );
				ibop += 2;
			}
			*ivmid /= *( ihs + 3 );
			
			iop += 3 * rank * ( rank + 1 );
		}
		
		*iv = 2 * *ivmid - *iv;
		--ic, ++ivmid, ++iv, ihs += 4;
	}
	
	while ( ic >= 0 ) {
		*ivmid = ( *ihs - *( ihs + 2 ) * *( ivmid - 1 ) ) / *( ihs + 3 );
		*iv = 2 * *ivmid - *iv;
		
		--ic, ++ivmid, ++iv, ihs += 4;
	}
}
