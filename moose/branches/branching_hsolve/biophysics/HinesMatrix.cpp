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
#include "NeuroScanBase.h"

void NeuroScanBase::createMatrix( Id seed ) {
	walkTree( seed );
	doHinesNumbering( );
	readCompartmentFields( );
	
	makeJunctions( );
	makeMatrix( );
	makeOperands( );
}

// Stage 1
void NeuroScanBase::walkTree( Id seed ) {
	// Find leaf node
	Id previous;
	vector< Id > recent;
	findAdjacent( seed, recent );
	if ( recent.size() != 1 )
		while ( !recent.empty() ) {
			previous = seed;
			seed = recent[ 0 ];
			findAdjacent( seed, previous, recent );
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
			findAdjacent( current, above, cstack.back() );
		}
	}
}

// Stage 2
void NeuroScanBase::doHinesNumbering( ) {
	reverse( compartmentId_.begin(), compartmentId_.end() );
	
	for ( unsigned int ic = 0; ic < compartmentId_.size(); ++ic )
		hinesIndex_[ compartmentId_[ ic ] ] = ic;
}

void NeuroScanBase::readCompartmentFields( ) {
	double Vm, Cm, Em, Rm, Ra, inject;
	
	N_ = compartmentId_.size();
	VMid_.resize( N_ );
	
	for ( unsigned int ic = 0; ic < N_; ++ic ) {
		field( compartmentId_[ ic ], "initVm", Vm );
		field( compartmentId_[ ic ], "Cm", Cm );
		field( compartmentId_[ ic ], "Em", Em );
		field( compartmentId_[ ic ], "Rm", Rm );
		field( compartmentId_[ ic ], "Ra", Ra );
		field( compartmentId_[ ic ], "inject", inject );
		
		V_.push_back( Vm );
		CmByDt_.push_back( 2.0 * Cm / dt_ );
		EmByRm_.push_back( Em / Rm );
		Ga_.push_back( 2.0 / Ra );
		inject_.push_back( inject );
	}
}

bool groupCompare(
	const vector< unsigned int >& A,
	const vector< unsigned int >& B )
{
	if ( A.size() && B.size() )
		return A[ 0 ] < B[ 0 ];
	return 0;
}

// Stage 3
void NeuroScanBase::makeJunctions( ) {
	// 3.1
	vector< Id > group;
	vector< Id >::iterator ig;
	for ( unsigned int ic = 0; ic < compartmentId_.size(); ++ic ) {
		const Id& parent = compartmentId_[ ic ];
		group.clear();
		findChildren( parent, group );
		
		if ( group.size() > 1 ) {
			group.push_back( parent );
			
			junctionGroup_.resize( junctionGroup_.size() + 1 );
			for ( ig = group.begin(); ig != group.end(); ++ig )
				junctionGroup_.back().push_back( hinesIndex_[ *ig ] );
		}
	}
	
	// 3.2
	vector< vector< unsigned int > >::iterator grp;
	for ( grp = junctionGroup_.begin(); grp != junctionGroup_.end(); ++grp )
		sort( grp->begin(), grp->end() );
	
	sort( junctionGroup_.begin(), junctionGroup_.end(), groupCompare );
	
	// 3.3
	int index;
	int rank;
	for ( grp = junctionGroup_.begin(); grp != junctionGroup_.end(); ++grp )
		// Loop uptil penultimate compartment in group
		for ( unsigned int c = 0; c < grp->size() - 1; ++c ) {
			index = ( *grp )[ c ];
			rank = grp->size() - c;
			branch_.push_back( BranchStruct( index, rank ) );
			
			groupNumber_[ index ] = grp - junctionGroup_.begin();
		}
	
	sort( branch_.begin(), branch_.end() );
}

// Stage 4
void NeuroScanBase::makeMatrix( ) {
	Mlinear_.resize( 4 * N_, 0.0 );
	
	// Setting up Mlinear
	for ( unsigned int i = 0; i < N_; ++i )
		Mlinear_[ 4 * i + 2 ] = CmByDt_[ i ];
	
	double gi, gj, gij;
	vector< BranchStruct >::iterator branch = branch_.begin();
	for ( unsigned int i = 0; i < N_ - 1; ++i ) {
		if ( branch_.size() && i == branch->index ) {
			++branch;
			continue;
		}
		
		gi = Ga_[ i ];
		gj = Ga_[ i + 1 ];
		gij = gi * gj / ( gi + gj );
		
		Mlinear_[ 4 * i + 1 ] = -gij;
		Mlinear_[ 4 * i + 2 ] += gij;
		Mlinear_[ 4 * i + 6 ] += gij;
	}
	
	vector< vector< unsigned int > >::iterator group;
	vector< unsigned int >::iterator i;
	for ( group = junctionGroup_.begin(); group != junctionGroup_.end(); ++group ) {
		double gsum = 0.0;
		
		for ( i = group->begin(); i != group->end(); ++i )
			gsum += Ga_[ *i ];
		
		for ( i = group->begin(); i != group->end(); ++i ) {
			gi = Ga_[ *i ];
			
			Mlinear_[ 4 * *i + 2 ] += gi * ( 1.0 - gi / gsum );
		}
	}
	
	// Setting up Mbranch
	//~ double g0, g1, g2;
	//~ double g01, g12, g20;
	vector< unsigned int >::iterator j;
	for ( group = junctionGroup_.begin(); group != junctionGroup_.end(); ++group ) {
		double gsum = 0.0;
		
		for ( i = group->begin(); i != group->end(); ++i )
			gsum += Ga_[ *i ];
		
		//~ const vector< int >& member = ( *group );
		//~ if ( group->size() == 3 ) {
			//~ g0 = Ga_[ member[ 0 ] ];
			//~ g1 = Ga_[ member[ 1 ] ];
			//~ g2 = Ga_[ member[ 2 ] ];
			//~ 
			//~ g01 = g0 * g1 / gsum;
			//~ g12 = g1 * g2 / gsum;
			//~ g20 = g2 * g0 / gsum;
			//~ 
			//~ Mbranch_.push_back( -g01 );
			//~ operandBase_[ member[ 0 ] ] = &( Mbranch_.back() );
			//~ Mbranch_.push_back( -g20 );
			//~ 
			//~ Mbranch_.push_back( -g12 );
			//~ operandBase_[ member[ 1 ] ] = &( Mbranch_.back() );
			//~ Mbranch_.push_back( -g12 ); // g21 = g12
		//~ } else
		for ( i = group->begin(); i != group->end() - 1; ++i ) {
			int base = Mbranch_.size();
			
			for ( j = i + 1; j != group->end(); ++j ) {
				gij = Ga_[ *i ] * Ga_[ *j ] / gsum;
				
				Mbranch_.push_back( -gij );
				Mbranch_.push_back( -gij );
			}
			
			operandBase_[ *i ] = &Mbranch_[ base ];
		}
	}
	
	MbranchCopy_.assign( Mbranch_.begin(), Mbranch_.end() );
}

// Stage 5
void NeuroScanBase::makeOperands( ) {
	unsigned int index;
	unsigned int rank;
	unsigned int farIndex;
	double* base;
	vector< BranchStruct >::iterator branch;
	
	// Operands for forward-elimination
	for ( branch = branch_.begin(); branch != branch_.end(); ++branch ) {
		index = branch->index;
		rank = branch->rank;
		base = operandBase_[ index ];
		
		// This is the list of compartments connected at a junction.
		const vector< unsigned int >& group =
			junctionGroup_[ groupNumber_[ index ] ];
		
		if ( rank == 1 ) {
			operand_.push_back( base );
		} else if ( rank == 2 ) {
			// Select 2nd last member.
			farIndex = group[ group.size() - 2 ];
			
			operand_.push_back( &Mlinear_[ 4 * farIndex ] );
			operand_.push_back( base );
			operand_.push_back( &VMid_[ farIndex ] );
		} else {
			// Operations on diagonal elements and elements from B (as in Ax = B).
			int start = group.size() - rank;
			for ( unsigned int j = 0; j < rank; ++j ) {
				farIndex = group[ start + j ];
				
				// Diagonal elements
				operand_.push_back( &Mlinear_[ 4 * farIndex ] );
				operand_.push_back( base + 2 * j );
				operand_.push_back( base + 2 * j + 1 );
				
				// Elements from B
				operand_.push_back( &Mlinear_[ 4 * farIndex + 3 ] );
				operand_.push_back( &Mlinear_[ 4 * index + 3 ] );
				operand_.push_back( base + 2 * j + 1 );
			}
			
			// Operations on off-diagonal elements.
			double *left;
			double *above;
			double *target;
			
			// Upper triangle elements
			left = base + 1;
			target = base + 2 * rank;
			for ( unsigned int i = 1; i < rank; ++i ) {
				above = base + 2 * i;
				for ( unsigned int j = 0; j < rank - i; ++j ) {
					operand_.push_back( target );
					operand_.push_back( above );
					operand_.push_back( left );
					
					above += 2;
					target += 2;
				}
				left += 2;
			}
			
			// Lower triangle elements
			target = base + 2 * rank + 1;
			above = base;
			for ( unsigned int i = 1; i < rank; ++i ) {
				left = base + 2 * i + 1;
				for ( unsigned int j = 0; j < rank - i; ++j ) {
					operand_.push_back( target );
					operand_.push_back( above );
					operand_.push_back( left );
					
					target += 2;
					left += 2;
				}
				above += 2;
			}
		}
	}
	
	// Operands for backward substitution
	for ( branch = branch_.begin(); branch != branch_.end(); ++branch ) {
		if ( branch->rank < 3 )
			continue;
		
		index = branch->index;
		rank = branch->rank;
		base = operandBase_[ index ];
		
		// This is the list of compartments connected at a junction.
		const vector< unsigned int >& group =
			junctionGroup_[ groupNumber_[ index ] ];
		
		unsigned int start = group.size() - rank;
		for ( unsigned int j = 0; j < rank; ++j ) {
			farIndex = group[ start + j ];
			
			backOperand_.push_back( base + 2 * j );
			backOperand_.push_back( &VMid_[ farIndex ] );
		}
	}
}



		//~ if ( rank == 1 ) {
			//~ backOperand.push_back( base );
		//~ } else if ( rank == 2 ) {
			//~ // Select 2nd last member.
			//~ farIndex = group[ group.size() - 2 ];
			//~ 
			//~ backOperand_.push_back( base );
			//~ backOperand_.push_back( &VMid_[ farIndex ] );
		//~ } else {
			//~ int start = group.size() - rank;
			//~ for ( int j = 0; j < rank; ++j ) {
				//~ farIndex = group[ start + j ];
				//~ 
				//~ backOperand_.push_back( base + 2 * j );
				//~ backOperand_.push_back( &VMid_[ farIndex ] );
			//~ }
		//~ }
