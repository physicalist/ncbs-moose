/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include "HinesMatrix.h"
#include <sstream>

void HinesMatrix::setup(
	const vector< vector< unsigned int > >& children,
	const vector< double >& Ga,
	const vector< double >& CmByDt )
{
	nCompt_ = children.size();
	children_ = &children;
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
	for ( unsigned int i = 0; i < children_->size(); ++i ) {
		const vector< unsigned int >& c = ( *children_ )[ i ];
		
		// If a compartment has >1 children, it means that the cable is branching.
		if ( c.size() > 1 ) {
			// The "junction group" contains a list of all children..
			junctionGroup_.push_back( c );
			// ..and the compartment itself.
			junctionGroup_.back().push_back( i );
		}
	}
	
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
			rank = group->size() - c - 1;
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
	
	// Copy diagonal elements into their final locations
	for ( unsigned int i = 0; i < nCompt_; ++i )
		HS_[ 4 * i ] = HS_[ 4 * i + 2 ];
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

///////////////////////////////////////////////////////////////////////////
// Interface to matrix entries
///////////////////////////////////////////////////////////////////////////
double HinesMatrix::entry( unsigned int row, unsigned int col )
{
	if ( row >= nCompt_ || col >= nCompt_ )
		return 0.0;
	
	if ( row == col )
		return HS_[ 4 * row ];
	
	unsigned int smaller = row < col ? row : col;
	unsigned int bigger = row > col ? row : col;
	
	if ( groupNumber_.find( smaller ) == groupNumber_.end() ) {
		if ( bigger - smaller == 1 )
			return HS_[ 4 * smaller + 1 ];
		else
			return 0.0;
	} else {
		unsigned int groupNumber = groupNumber_[ smaller ];
		const vector< unsigned int >& group = junctionGroup_[ groupNumber ];
		unsigned int location, size;
		unsigned int smallRank, bigRank;
		
		if ( find( group.begin(), group.end(), bigger ) != group.end() ) {
			location = 0;
			for ( int i = 0; i < ( int )groupNumber; ++i ) {
				size = junctionGroup_[ i ].size();
				location += size * ( size - 1 );
			}
			
			size = group.size();
			smallRank = group.end() - find( group.begin(), group.end(), smaller ) - 1;
			bigRank = group.end() - find( group.begin(), group.end(), bigger ) - 1;
			location += size * ( size - 1 ) - smallRank * ( smallRank + 1 );
			location += 2 * ( smallRank - bigRank - 1 );
			
			if ( row == smaller )
				return HJ_[ location ];
			else
				return HJ_[ location + 1 ];
		} else {
			return 0.0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS

/**
 *  We test if the Hines' matrix is correctly setup for the following cell:
 * 
 *   Soma--->  15 - 14 - 13 - 12
 *              |    |
 *              |    L 11 - 10
 *              |
 *              L 16 - 17 - 18 - 19
 *                      |
 *                      L 9 - 8 - 7 - 6 - 5
 *                      |         |
 *                      |         L 4 - 3
 *                      |
 *                      L 2 - 1 - 0
 * 
 *  The numbers are the hines indices of compartments. Compartment X is the
 *  child of compartment Y if X is one level further away from the soma (#15)
 *  than Y. So #17 is the parent of #'s 2, 9 and 18.
 */
void testHinesMatrix()
{
	cout << "\nTesting HinesMatrix" << flush;
	
	unsigned int i;
	unsigned int j;
	unsigned int NCOMPT = 20;
	vector< double > Ga;
	vector< double > CmByDt;
	
	// The i-th entry in 'children' is the list of children of the i-th compt.
	vector< vector< unsigned int > > children;
	
	/* Each entry in 'coupling' is a list of electrically coupled compartments.
	 * These compartments could be linked at junctions, or even in linear segments
	 * of the cell.
	 */
	vector< vector< unsigned int > > coupling;
	
	/* This is the full reference matrix which will be compared to its sparse
	 *  implementation.
	 */
	vector< vector< double > > matrix;
	
	Ga.resize( NCOMPT );
	CmByDt.resize( NCOMPT );
	children.resize( NCOMPT );
	matrix.resize( NCOMPT );
	for ( i = 0; i < NCOMPT; ++i )
		matrix[ i ].resize( NCOMPT );
	
	for ( i = 0; i < NCOMPT; ++i ) {
		Ga[ i ] = 15.0 + 3.0 * i;
		CmByDt[ i ] = 500.0 + 200.0 * i * i;
	}
	
	children[ 1 ].push_back( 0 );
	children[ 2 ].push_back( 1 );
	children[ 4 ].push_back( 3 );
	children[ 6 ].push_back( 5 );
	children[ 7 ].push_back( 4 ); children[ 7 ].push_back( 6 );
	children[ 8 ].push_back( 7 );
	children[ 9 ].push_back( 8 );
	children[ 11 ].push_back( 10 );
	children[ 13 ].push_back( 12 );
	children[ 14 ].push_back( 11 ); children[ 14 ].push_back( 13 );
	children[ 15 ].push_back( 14 ); children[ 15 ].push_back( 16 );
	children[ 16 ].push_back( 17 );
	children[ 17 ].push_back( 2 ); children[ 17 ].push_back( 9 ); children[ 17 ].push_back( 18 );
	children[ 18 ].push_back( 19 );
	
	int co_array[ ] =
	{
		-1, 0, 1,
		-1, 1, 2,
		-1, 2, 9, 17, 18,
		-1, 3, 4,
		-1, 4, 6, 7,
		-1, 5, 6,
		-1, 7, 8,
		-1, 8, 9,
		-1, 10, 11,
		-1, 11, 13, 14,
		-1, 12, 13,
		-1, 14, 15, 16,
		-1, 16, 17,
		-1, 18, 19
	};
	
	for ( i = 0; i < sizeof( co_array ) / sizeof( int ); i++ )
		if ( co_array[ i ] == -1 )
			coupling.resize( coupling.size() + 1 );
		else		
			coupling.back().push_back( co_array[ i ] );
	
	// Setting diagonal elements
	for ( i = 0; i < NCOMPT; i++ )
		matrix[ i ][ i ] = CmByDt[ i ];
	
	double gi;
	vector< vector< unsigned int > >::iterator group;
	vector< unsigned int >::iterator ic;
	for ( group = coupling.begin(); group != coupling.end(); ++group ) {
		double gsum = 0.0;
		
		for ( ic = group->begin(); ic != group->end(); ++ic )
			gsum += Ga[ *ic ];
		
		for ( ic = group->begin(); ic != group->end(); ++ic ) {
			gi = Ga[ *ic ];
			
			matrix[ *ic ][ *ic ] += gi * ( 1.0 - gi / gsum );
		}
	}
	
	// Setting off-diagonal elements
	double gij;
	vector< unsigned int >::iterator jc;
	for ( group = coupling.begin(); group != coupling.end(); ++group ) {
		double gsum = 0.0;
		
		for ( ic = group->begin(); ic != group->end(); ++ic )
			gsum += Ga[ *ic ];
		
		for ( ic = group->begin(); ic != group->end() - 1; ++ic ) {
			for ( jc = ic + 1; jc != group->end(); ++jc ) {
				gij = Ga[ *ic ] * Ga[ *jc ] / gsum;
				
				matrix[ *ic ][ *jc ] = -gij;
				matrix[ *jc ][ *ic ] = -gij;
			}
		}
	}
	
	HinesMatrix H;
	H.setup( children, Ga, CmByDt );
	
	for ( i = 0; i < NCOMPT; ++i )
		for ( j = 0; j < NCOMPT; ++j ) {
			ostringstream error;
			error << "Testing Hines' Matrix: (" << i << ", " << j << ")";
			ASSERT(
				matrix[ i ][ j ] == H.entry( i, j ),
				error.str()
			);
		}
}

#endif // DO_UNIT_TESTS
