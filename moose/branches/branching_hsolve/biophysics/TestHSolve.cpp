/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifdef DO_UNIT_TESTS

#include <vector>
using namespace std;

extern void testBioScan(); // Defined in BioScan.cpp
extern void testHinesMatrix(); // Defined in HinesMatrix.cpp
extern void testHSolvePassive(); // Defined in HSolvePassive.cpp

void testHSolve()
{
	testBioScan();
	testHinesMatrix();
	//~ testHSolvePassive();
}

//////////////////////////////////////////////////////////////////////////////
// Helper functions called in testHinesMatrix, testHSolvePassive, etc.
//////////////////////////////////////////////////////////////////////////////

void array2vec(
	int *array,
	int size,
	vector< vector< unsigned int > >& vec )
{
	vec.clear();
	
	for ( int i = 0; i < size; i++ )
		if ( array[ i ] == -1 )
			vec.resize( vec.size() + 1 );
		else		
			vec.back().push_back( array[ i ] );
}

void makeFullMatrix(
	const vector< vector< unsigned int > >& children,
	const vector< double >& Ga,
	const vector< double >& CmByDt,
	vector< vector< double > >& matrix )
{
	unsigned int size = children.size();
	
	/* Each entry in 'coupled' is a list of electrically coupled compartments.
	 * These compartments could be linked at junctions, or even in linear segments
	 * of the cell.
	 */
	vector< vector< unsigned int > > coupled;
	for ( unsigned int i = 0; i < children.size(); i++ )
		if ( children[ i ].size() >= 1 ) {
			coupled.push_back( children[ i ] );
			coupled.back().push_back( i );
		}
	
	matrix.clear( );
	matrix.resize( size );
	for ( unsigned int i = 0; i < size; ++i )
		matrix[ i ].resize( size );
	
	// Setting diagonal elements
	for ( unsigned int i = 0; i < size; i++ )
		matrix[ i ][ i ] = CmByDt[ i ];
	
	double gi;
	vector< vector< unsigned int > >::iterator group;
	vector< unsigned int >::iterator ic;
	for ( group = coupled.begin(); group != coupled.end(); ++group ) {
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
	for ( group = coupled.begin(); group != coupled.end(); ++group ) {
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
}

void updateIndices(
	vector< vector< unsigned int > >& v,
	const vector< unsigned int >& permutation )
{
	for ( unsigned int i = 0; i < v.size(); i++ )
		for ( unsigned int j = 0; j < v[ i ].size(); j++ )
			v[ i ][ j ] = permutation[ v[ i ][ j ] ];
}

#endif
