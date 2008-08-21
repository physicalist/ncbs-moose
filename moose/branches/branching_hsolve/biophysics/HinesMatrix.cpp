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
	refresh( );
	
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
		
		if ( c.size() == 0 )
			continue;
		
		if ( c.size() == 1 ) {
			int diff = ( int )( c[ 0 ] ) - i;
		
			if ( diff == 1 || diff == -1 )
				continue;
		}
		
		// "coupled" contains a list of all children..
		coupled_.push_back( c );
		// ..and the parent compartment itself.
		coupled_.back().push_back( i );
	}
	
	// 3.2
	vector< vector< unsigned int > >::iterator group;
	for ( group = coupled_.begin(); group != coupled_.end(); ++group )
		sort( group->begin(), group->end() );
	
	sort( coupled_.begin(), coupled_.end(), groupCompare );
	
	// 3.3
	unsigned int index;
	unsigned int rank;
	for ( group = coupled_.begin(); group != coupled_.end(); ++group )
		// Loop uptil penultimate compartment in group
		for ( unsigned int c = 0; c < group->size() - 1; ++c ) {
			index = ( *group )[ c ];
			rank = group->size() - c - 1;
			junction_.push_back( JunctionStruct( index, rank ) );
			
			groupNumber_[ index ] = group - coupled_.begin();
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
		if ( junction_.size() &&
		     junction < junction_.end() &&
		     i == junction->index )
	{
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
	for ( group = coupled_.begin(); group != coupled_.end(); ++group ) {
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
	unsigned int size = 0;
	unsigned int rank;
	for ( group = coupled_.begin(); group != coupled_.end(); ++group ) {
		rank = group->size() - 1;
		size += rank * ( rank + 1 );
	}
	
	HJ_.reserve( size );
	
	for ( group = coupled_.begin(); group != coupled_.end(); ++group ) {
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
			
			//~ operandBase_[ *i ] = &HJ_[ base ];
			operandBase_[ *i ] = HJ_.begin() + base;
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
	vdIterator base;
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
			coupled_[ groupNumber_[ index ] ];
		
		if ( rank == 1 ) {
			operand_.push_back( base );
			
			// Select last member.
			farIndex = group[ group.size() - 1 ];
			operand_.push_back( HS_.begin() + 4 * farIndex );
			operand_.push_back( VMid_.begin() + farIndex );
		} else if ( rank == 2 ) {
			operand_.push_back( base );
			
			// Select 2nd last member.
			farIndex = group[ group.size() - 2 ];
			operand_.push_back( HS_.begin() + 4 * farIndex );
			operand_.push_back( VMid_.begin() + farIndex );
			
			// Select last member.
			farIndex = group[ group.size() - 1 ];
			operand_.push_back( HS_.begin() + 4 * farIndex );
			operand_.push_back( VMid_.begin() + farIndex );
		} else {
			// Operations on diagonal elements and elements from B (as in Ax = B).
			int start = group.size() - rank;
			for ( unsigned int j = 0; j < rank; ++j ) {
				farIndex = group[ start + j ];
				
				// Diagonal elements
				operand_.push_back( HS_.begin() + 4 * farIndex );
				operand_.push_back( base + 2 * j );
				operand_.push_back( base + 2 * j + 1 );
				
				// Elements from B
				operand_.push_back( HS_.begin() + 4 * farIndex + 3 );
				operand_.push_back( HS_.begin() + 4 * farIndex + 3 );
				operand_.push_back( base + 2 * j + 1 );
			}
			
			// Operations on off-diagonal elements.
			vdIterator left;
			vdIterator above;
			vdIterator target;
			
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
			coupled_[ groupNumber_[ index ] ];
		
		unsigned int start = group.size() - rank;
		for ( unsigned int j = 0; j < rank; ++j ) {
			farIndex = group[ start + j ];
			
			backOperand_.push_back( base + 2 * j );
			backOperand_.push_back( VMid_.begin() + farIndex );
		}
	}
}

void HinesMatrix::refresh( )
{
	nCompt_ = 0;
	junction_.clear( );
	HS_.clear( );
	HJ_.clear( );
	HJCopy_.clear( );
	VMid_.clear( );
	operand_.clear( );
	backOperand_.clear( );
	stage_ = 0;
	
	children_ = 0;
	Ga_ = 0;
	coupled_.clear( );
	operandBase_.clear( );
	groupNumber_.clear( );
}

///////////////////////////////////////////////////////////////////////////
// Interface to matrix entries
///////////////////////////////////////////////////////////////////////////
double HinesMatrix::getA( unsigned int row, unsigned int col )
{
	if ( stage_ == 1 && row > col )
		return 0.0;
	
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
		const vector< unsigned int >& group = coupled_[ groupNumber ];
		unsigned int location, size;
		unsigned int smallRank, bigRank;
		
		if ( find( group.begin(), group.end(), bigger ) != group.end() ) {
			location = 0;
			for ( int i = 0; i < ( int )groupNumber; ++i ) {
				size = coupled_[ i ].size();
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

double HinesMatrix::getB( unsigned int row )
{
	return HS_[ 4 * row + 3 ];
}

double HinesMatrix::getVMid( unsigned int row )
{
	return VMid_[ row ];
}

///////////////////////////////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS

#include "TestHSolve.h"

void testHinesMatrix()
{
	cout << "\nTesting HinesMatrix" << flush;
	vector< unsigned int > N;
	vector< int* > childArray;
	vector< unsigned int > childArraySize;
	
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
	
	N.push_back( 20 );
	
	int childArray_1[ ] =
	{
		/* c0  */  -1, 
		/* c1  */  -1, 0,
		/* c2  */  -1, 1,
		/* c3  */  -1, 
		/* c4  */  -1, 3,
		/* c5  */  -1, 
		/* c6  */  -1, 5,
		/* c7  */  -1, 4, 6,
		/* c8  */  -1, 7,
		/* c9  */  -1, 8,
		/* c10 */  -1, 
		/* c11 */  -1, 10,
		/* c12 */  -1, 
		/* c13 */  -1, 12,
		/* c14 */  -1, 11, 13,
		/* c15 */  -1, 14, 16,
		/* c16 */  -1, 17,
		/* c17 */  -1, 2, 9, 18,
		/* c18 */  -1, 19,
		/* c19 */  -1, 
	};
	
	childArray.push_back( childArray_1 );
	childArraySize.push_back( sizeof( childArray_1 ) / sizeof( int ) );
	
	/**
	 *  Cell 2:
	 * 
	 *             3
	 *             |
	 *   Soma--->  2
	 *            / \
	 *           /   \
	 *          1     0
	 * 
	 */
	
	N.push_back( 4 );
	
	int childArray_2[ ] =
	{
		/* c0  */  -1, 
		/* c1  */  -1,
		/* c2  */  -1, 0, 1, 3,
		/* c3  */  -1, 
	};
	
	childArray.push_back( childArray_2 );
	childArraySize.push_back( sizeof( childArray_2 ) / sizeof( int ) );
	
	/**
	 *  Cell 3:
	 * 
	 *             3
	 *             |
	 *             2
	 *            / \
	 *           /   \
	 *          1     0  <--- Soma
	 * 
	 */
	
	N.push_back( 4 );
	
	int childArray_3[ ] =
	{
		/* c0  */  -1, 2, 
		/* c1  */  -1,
		/* c2  */  -1, 1, 3,
		/* c3  */  -1, 
	};
	
	childArray.push_back( childArray_3 );
	childArraySize.push_back( sizeof( childArray_3 ) / sizeof( int ) );
	
	/**
	 *  Cell 4:
	 * 
	 *             3  <--- Soma
	 *             |
	 *             2
	 *            / \
	 *           /   \
	 *          1     0
	 * 
	 */
	
	N.push_back( 4 );
	
	int childArray_4[ ] =
	{
		/* c0  */  -1,
		/* c1  */  -1,
		/* c2  */  -1, 0, 1,
		/* c3  */  -1, 2,
	};
	
	childArray.push_back( childArray_4 );
	childArraySize.push_back( sizeof( childArray_4 ) / sizeof( int ) );
	
	/**
	 *  Cell 5:
	 * 
	 *             1  <--- Soma
	 *             |
	 *             2
	 *            / \
	 *           4   0
	 *          / \
	 *         3   5
	 * 
	 */
	
	N.push_back( 6 );
	
	int childArray_5[ ] =
	{
		/* c0  */  -1,
		/* c1  */  -1, 2,
		/* c2  */  -1, 0, 4,
		/* c3  */  -1,
		/* c4  */  -1, 3, 5,
		/* c5  */  -1,
	};
	
	childArray.push_back( childArray_5 );
	childArraySize.push_back( sizeof( childArray_5 ) / sizeof( int ) );
	
	/**
	 *  Cell 6:
	 * 
	 *             3  <--- Soma
	 *             L 4
	 *               L 6
	 *               L 5
	 *               L 2
	 *               L 1
	 *               L 0
	 * 
	 */
	
	N.push_back( 7 );
	
	int childArray_6[ ] =
	{
		/* c0  */  -1,
		/* c1  */  -1,
		/* c2  */  -1,
		/* c3  */  -1, 4,
		/* c4  */  -1, 0, 1, 2, 5, 6,
		/* c5  */  -1,
		/* c6  */  -1,
	};
	
	childArray.push_back( childArray_6 );
	childArraySize.push_back( sizeof( childArray_6 ) / sizeof( int ) );
	
	/**
	 *  Cell 7: Single compartment
	 */
	
	N.push_back( 1 );
	
	int childArray_7[ ] =
	{
		/* c0  */  -1,
	};
	
	childArray.push_back( childArray_7 );
	childArraySize.push_back( sizeof( childArray_7 ) / sizeof( int ) );
	
	/**
	 *  Cell 8: 3 compartments; soma is in the middle.
	 */
	
	N.push_back( 3 );
	
	int childArray_8[ ] =
	{
		/* c0  */  -1,
		/* c1  */  -1, 0, 2,
		/* c2  */  -1,
	};
	
	childArray.push_back( childArray_8 );
	childArraySize.push_back( sizeof( childArray_8 ) / sizeof( int ) );
	
	/**
	 *  Cell 9: 3 compartments; first compartment is soma.
	 */
	
	N.push_back( 3 );
	
	int childArray_9[ ] =
	{
		/* c0  */  -1, 1,
		/* c1  */  -1, 2,
		/* c2  */  -1,
	};
	
	childArray.push_back( childArray_9 );
	childArraySize.push_back( sizeof( childArray_9 ) / sizeof( int ) );
	
	////////////////////////////////////////////////////////////////////////////
	// Run tests
	////////////////////////////////////////////////////////////////////////////
	HinesMatrix H;
	vector< double > Ga;
	vector< double > CmByDt;
	
	// The i-th entry in 'children' is the list of children of the i-th compt.
	vector< vector< unsigned int > > children;
	
	/* This is the full reference matrix which will be compared to its sparse
	 *  implementation.
	 */
	vector< vector< double > > matrix;
	
	unsigned int i;
	unsigned int j;
	unsigned int nCompt;
	int* array;
	unsigned int arraySize;
	for ( unsigned int cell = 0; cell < childArray.size(); cell++ ) {
		nCompt = N[ cell ];
		array = childArray[ cell ];
		arraySize = childArraySize[ cell ];
		
		// Prepare cell
		Ga.clear();
		CmByDt.clear();
		for ( i = 0; i < nCompt; ++i ) {
			Ga.push_back( 15.0 + 3.0 * i );
			CmByDt.push_back( 500.0 + 200.0 * i * i );
		}
		
		children.clear();
		for ( unsigned int a = 0; a < arraySize; a++ )
			if ( array[ a ] == -1 )
				children.resize( children.size() + 1 );
			else		
				children.back().push_back( array[ a ] );
		
		// Prepare local matrix
		makeFullMatrix(	children, Ga, CmByDt, matrix );
		
		// Prepare sparse matrix
		H.setup( children, Ga, CmByDt );
		
		// Compare matrices
		for ( i = 0; i < nCompt; ++i )
			for ( j = 0; j < nCompt; ++j ) {
				ostringstream error;
				error << "Testing Hines' Matrix: (" << i << ", " << j << ")";
				ASSERT(
					matrix[ i ][ j ] == H.getA( i, j ),
					error.str()
				);
			}
	}
}

#endif // DO_UNIT_TESTS
