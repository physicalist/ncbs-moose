/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "HSolvePassive.h"

void HSolvePassive::setup( Id seed, double dt ) {
	dt_ = dt;
	
	walkTree( seed );
	initialize( );
	storeTree( );
	
	this->HinesMatrix::setup( tree_, Ga_ );
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
	nCompt_ = compartmentId_.size();
	
	double Vm, Cm, Em, Rm, Ra, inject;
	vector< Id >::iterator ic;
	for ( ic = compartmentId_.begin(); ic != compartmentId_.end(); ++ic ) {
		get< double >( *ic, "initVm", Vm );
		get< double >( *ic, "Cm", Cm );
		get< double >( *ic, "Em", Em );
		get< double >( *ic, "Rm", Rm );
		get< double >( *ic, "Ra", Ra );
		get< double >( *ic, "inject", inject );
		
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
		findChildren( parent, children );
		
		tree_.resize( tree_.size() + 1 );
		// Push hines' index of parent
		tree_.back().push_back( ic );
		
		// Push hines' indices of children
		for ( child = children.begin(); child != children.end(); ++child )
			tree_.back().push_back(
				hinesIndex[ *child ] );
	}
}

//////////////////////////////////////////////////////////////////////
// Numerical integration
//////////////////////////////////////////////////////////////////////

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

