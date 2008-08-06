/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <algorithm>
#include <vector>
#include <map>

using namespace std;

#include "HinesMatrix.h"

void HinesMatrix::setup(
	const vector< vector< unsigned int > >& tree,
	const vector< double >& Ga,
	const vector< double >& CmByDt )
{
	nCompt_ = tree.size();
	tree_ = &tree;
	Ga_ = &Ga;
	CmByDt_ = &CmByDt;
	
	makeJunctions( );
	makeMatrix( );
	makeOperands( );
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
void HinesMatrix::makeJunctions( ) {
	// 3.1
	vector< vector< unsigned int > >::const_iterator i;
	for ( i = tree_->begin(); i != tree_->end(); ++i )
		// If this compartment has children
		if ( i->size() > 1 )
			junctionGroup_.push_back( *i );
	
	// 3.2
	vector< vector< unsigned int > >::iterator group;
	for ( group = junctionGroup_.begin(); group != junctionGroup_.end(); ++group )
		sort( group->begin(), group->end() );
	
	sort( junctionGroup_.begin(), junctionGroup_.end(), groupCompare );
	
	// 3.3
	unsigned int index;
	unsigned int rank;
	for ( group = junctionGroup_.begin(); group != junctionGroup_.end(); ++group )
		// Loop uptil penultimate compartment in group
		for ( unsigned int c = 0; c < group->size() - 1; ++c ) {
			index = ( *group )[ c ];
			rank = group->size() - c;
			junction_.push_back( JunctionStruct( index, rank ) );
			
			groupNumber_[ index ] = group - junctionGroup_.begin();
		}
	
	sort( junction_.begin(), junction_.end() );
}

// Stage 4
void HinesMatrix::makeMatrix( ) {
	const vector< double >& Ga = *Ga_;
	const vector< double >& CmByDt = *CmByDt_;
	
	// Setting up HS
	HS_.resize( 4 * nCompt_, 0.0 );
	for ( unsigned int i = 0; i < nCompt_; ++i )
		HS_[ 4 * i + 2 ] = CmByDt[ i ];
	
	double gi, gj, gij;
	vector< JunctionStruct >::iterator junction = junction_.begin();
	for ( unsigned int i = 0; i < nCompt_ - 1; ++i ) {
		if ( junction_.size() && i == junction->index ) {
			++junction;
			continue;
		}
		
		gi = Ga[ i ];
		gj = Ga[ i + 1 ];
		gij = gi * gj / ( gi + gj );
		
		HS_[ 4 * i + 1 ] = -gij;
		HS_[ 4 * i + 2 ] += gij;
		HS_[ 4 * i + 6 ] += gij;
	}
	
	vector< vector< unsigned int > >::iterator group;
	vector< unsigned int >::iterator i;
	for ( group = junctionGroup_.begin(); group != junctionGroup_.end(); ++group ) {
		double gsum = 0.0;
		
		for ( i = group->begin(); i != group->end(); ++i )
			gsum += Ga[ *i ];
		
		for ( i = group->begin(); i != group->end(); ++i ) {
			gi = Ga[ *i ];
			
			HS_[ 4 * *i + 2 ] += gi * ( 1.0 - gi / gsum );
		}
	}
	
	// Setting up HJ
	vector< unsigned int >::iterator j;
	for ( group = junctionGroup_.begin(); group != junctionGroup_.end(); ++group ) {
		double gsum = 0.0;
		
		for ( i = group->begin(); i != group->end(); ++i )
			gsum += Ga[ *i ];
		
		for ( i = group->begin(); i != group->end() - 1; ++i ) {
			int base = HJ_.size();
			
			for ( j = i + 1; j != group->end(); ++j ) {
				gij = Ga[ *i ] * Ga[ *j ] / gsum;
				
				HJ_.push_back( -gij );
				HJ_.push_back( -gij );
			}
			
			operandBase_[ *i ] = &HJ_[ base ];
		}
	}
	
	// Create copy of HJ
	HJCopy_.assign( HJ_.begin(), HJ_.end() );
}

// Stage 5
void HinesMatrix::makeOperands( ) {
	unsigned int index;
	unsigned int rank;
	unsigned int farIndex;
	double* base;
	vector< JunctionStruct >::iterator junction;
	
	// Allocate space in VMid. Needed, since we will store pointers to its
	// elements below.
	VMid_.resize( nCompt_ );
	
	// Operands for forward-elimination
	for ( junction = junction_.begin(); junction != junction_.end(); ++junction ) {
		index = junction->index;
		rank = junction->rank;
		base = operandBase_[ index ];
		
		// This is the list of compartments connected at a junction.
		const vector< unsigned int >& group =
			junctionGroup_[ groupNumber_[ index ] ];
		
		if ( rank == 1 ) {
			operand_.push_back( base );
		} else if ( rank == 2 ) {
			// Select 2nd last member.
			farIndex = group[ group.size() - 2 ];
			
			operand_.push_back( &HS_[ 4 * farIndex ] );
			operand_.push_back( base );
			operand_.push_back( &VMid_[ farIndex ] );
		} else {
			// Operations on diagonal elements and elements from B (as in Ax = B).
			int start = group.size() - rank;
			for ( unsigned int j = 0; j < rank; ++j ) {
				farIndex = group[ start + j ];
				
				// Diagonal elements
				operand_.push_back( &HS_[ 4 * farIndex ] );
				operand_.push_back( base + 2 * j );
				operand_.push_back( base + 2 * j + 1 );
				
				// Elements from B
				operand_.push_back( &HS_[ 4 * farIndex + 3 ] );
				operand_.push_back( &HS_[ 4 * index + 3 ] );
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
	for ( junction = junction_.begin(); junction != junction_.end(); ++junction ) {
		if ( junction->rank < 3 )
			continue;
		
		index = junction->index;
		rank = junction->rank;
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
