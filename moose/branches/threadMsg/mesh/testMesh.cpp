/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "../shell/Shell.h"
#include "Boundary.h"
#include "MeshEntry.h"
#include "ChemMesh.h"
#include "CylMesh.h"
#include "CubeMesh.h"

/**
 * Low-level tests for the CylMesh object: No MOOSE calls involved.
 */
void testCylMesh()
{
	CylMesh cm;
	assert( cm.getMeshType( 0 ) == CYL );
	assert( cm.getMeshDimensions( 0 ) == 3 );
	assert( cm.getDimensions() == 3 );

	vector< double > coords( 9 );
	coords[0] = 1; // X0
	coords[1] = 2; // Y0
	coords[2] = 3; // Z0

	coords[3] = 3; // X1
	coords[4] = 5; // Y1
	coords[5] = 7; // Z1

	coords[6] = 1; // R0
	coords[7] = 2; // R1

	coords[8] = 1; // lambda

	cm.setCoords( coords );

	assert( doubleEq( cm.getX0(), 1 ) );
	assert( doubleEq( cm.getY0(), 2 ) );
	assert( doubleEq( cm.getZ0(), 3 ) );
	assert( doubleEq( cm.getR0(), 1 ) );

	assert( doubleEq( cm.getX1(), 3 ) );
	assert( doubleEq( cm.getY1(), 5 ) );
	assert( doubleEq( cm.getZ1(), 7 ) );
	assert( doubleEq( cm.getR1(), 2 ) );

	assert( doubleEq( cm.getLambda(), sqrt( 29 ) / 5.0 ) );

	cm.setX0( 2 );
	cm.setY0( 3 );
	cm.setZ0( 4 );
	cm.setR0( 2 );

	cm.setX1( 4 );
	cm.setY1( 6 );
	cm.setZ1( 8 );
	cm.setR1( 3 );

	cm.setLambda( 2.0 );

	vector< double > temp = cm.getCoords();
	assert( temp.size() == 9 );
	// Can't check on the last coord as it is lambda, it changes.
	for ( unsigned int i = 0; i < temp.size() - 1; ++i )
		assert( doubleEq( temp[i], coords[i] + 1 ) );
	
	double totLen = sqrt( 29.0 );
	assert( doubleEq( cm.getTotLength() , totLen ) );

	cm.setLambda( 1.0 );
	assert( cm.getNumEntries() == 5 );
	assert( doubleEq( cm.getLambda(), totLen / 5 ) );

	///////////////////////////////////////////////////////////////
	assert( doubleEq( cm.getMeshEntrySize( 2 ), 2.5 * 2.5 * PI * totLen / 5 ) );

	///////////////////////////////////////////////////////////////
	// LenSlope/totLen = 0.016 = 
	// 	1/numEntries * (r1-r0)/numEntries * 2/(r0+r1) = 1/25 * 1 * 2/5
	// Here are the fractional positions
	// part0 = 1/5 - 0.032: end= 0.2 - 0.032
	// part1 = 1/5 - 0.016: end = 0.4 - 0.048
	// part2 = 1/5			: end = 0.6 - 0.048
	// part3 = 1/5 + 0.016	: end = 0.8 - 0.032
	// part4 = 1/5 + 0.032	: end = 1

	coords = cm.getCoordinates( 2 );
	assert( coords.size() == 10 );
	assert( doubleEq( coords[0], 2 + (0.4 - 0.048) * 2 ) );
	assert( doubleEq( coords[1], 3 + (0.4 - 0.048) * 3 ) );
	assert( doubleEq( coords[2], 4 + (0.4 - 0.048) * 4 ) );

	assert( doubleEq( coords[3], 2 + (0.6 - 0.048) * 2 ) );
	assert( doubleEq( coords[4], 3 + (0.6 - 0.048) * 3 ) );
	assert( doubleEq( coords[5], 4 + (0.6 - 0.048) * 4 ) );

	assert( doubleEq( coords[6], 2.4 ) );
	assert( doubleEq( coords[7], 2.6 ) );

	///////////////////////////////////////////////////////////////
	vector< unsigned int > neighbors = cm.getNeighbors( 2 );
	assert( neighbors.size() == 2 );
	assert( neighbors[0] == 1 );
	assert( neighbors[1] == 3 );

	///////////////////////////////////////////////////////////////
	coords = cm.getDiffusionArea( 2 );
	assert( coords.size() == 2 );
	assert( doubleEq( coords[0], 2.4 * 2.4 * PI ) );
	assert( doubleEq( coords[1], 2.6 * 2.6 * PI ) );

	cout << "." << flush;
}


/**
 * mid-level tests for the CylMesh object, using MOOSE calls.
 */
void testMidLevelCylMesh()
{
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	vector< unsigned int > dims( 1, 1 );

	Id cylId = s->doCreate( "CylMesh", Id(), "cyl", dims, 0 );
	Id meshId( cylId.value() + 1 );

	vector< double > coords( 9 );
	coords[0] = 1; // X0
	coords[1] = 2; // Y0
	coords[2] = 3; // Z0

	coords[3] = 3; // X1
	coords[4] = 5; // Y1
	coords[5] = 7; // Z1

	coords[6] = 1; // R0
	coords[7] = 2; // R1

	coords[8] = 1; // lambda

	bool ret = Field< vector< double > >::set( cylId, "coords", coords );
	assert( ret );

	assert( doubleEq( Field< double >::get( cylId, "x0" ), 1 ) );
	assert( doubleEq( Field< double >::get( cylId, "y0" ), 2 ) );
	assert( doubleEq( Field< double >::get( cylId, "z0" ), 3 ) );
	assert( doubleEq( Field< double >::get( cylId, "x1" ), 3 ) );
	assert( doubleEq( Field< double >::get( cylId, "y1" ), 5 ) );
	assert( doubleEq( Field< double >::get( cylId, "z1" ), 7 ) );
	assert( doubleEq( Field< double >::get( cylId, "r0" ), 1 ) );
	assert( doubleEq( Field< double >::get( cylId, "r1" ), 2 ) );

	ret = Field< double >::set( cylId, "lambda", 1 );
	assert( ret );

	assert( meshId()->dataHandler()->localEntries() == 5 );

	unsigned int n = Field< unsigned int >::get( cylId, "num_meshEntries" );
	assert( n == 5 );

	ObjId oid( meshId, DataId( 0, 2 ) );

	double totLen = sqrt( 29.0 );
	assert( doubleEq( Field< double >::get( oid, "size" ),
		1.5 * 1.5 * PI * totLen / 5 ) );

	vector< unsigned int > neighbors = 
		Field< vector< unsigned int > >::get( oid, "neighbors" );
	assert( neighbors.size() == 2 );
	assert( neighbors[0] = 1 );
	assert( neighbors[1] = 3 );

	s->doDelete( cylId );

	cout << "." << flush;
}

/**
 * Low-level tests for the CubeMesh object: No MOOSE calls involved.
 */
void testCubeMesh()
{
	CubeMesh cm;
	assert( cm.getMeshType( 0 ) == CUBOID );
	assert( cm.getMeshDimensions( 0 ) == 3 );
	assert( cm.getDimensions() == 3 );

	vector< double > coords( 9 );
	coords[0] = 0; // X0
	coords[1] = 0; // Y0
	coords[2] = 0; // Z0

	coords[3] = 2; // X1
	coords[4] = 4; // Y1
	coords[5] = 8; // Z1

	coords[6] = 1; // DX
	coords[7] = 1; // DY
	coords[8] = 1; // DZ

	cm.setCoords( coords );

	assert( doubleEq( cm.getX0(), 0 ) );
	assert( doubleEq( cm.getY0(), 0 ) );
	assert( doubleEq( cm.getZ0(), 0 ) );

	assert( doubleEq( cm.getX1(), 2 ) );
	assert( doubleEq( cm.getY1(), 4 ) );
	assert( doubleEq( cm.getZ1(), 8 ) );

	assert( doubleEq( cm.getDx(), 1 ) );
	assert( doubleEq( cm.getDy(), 1 ) );
	assert( doubleEq( cm.getDz(), 1 ) );

	cm.setX0( 1 );
	cm.setY0( 2 );
	cm.setZ0( 4 );

	cm.setX1( 5 );
	cm.setY1( 6 );
	cm.setZ1( 8 );

	vector< double > temp = cm.getCoords();
	assert( temp.size() == 9 );
	assert( doubleEq( temp[0], 1 ) );
	assert( doubleEq( temp[1], 2 ) );
	assert( doubleEq( temp[2], 4 ) );
	assert( doubleEq( temp[3], 5 ) );
	assert( doubleEq( temp[4], 6 ) );
	assert( doubleEq( temp[5], 8 ) );
	assert( doubleEq( temp[6], 1 ) );
	assert( doubleEq( temp[7], 1 ) );
	assert( doubleEq( temp[8], 1 ) );

	vector< unsigned int > m2s;
	vector< unsigned int > s2m( 64, ~0);
	for ( unsigned int i = 0; i < 4; ++i ) {
		for ( unsigned int j = 0; j < 4; ++j ) {
			for ( unsigned int k = 0; k < 4; ++k ) {
				unsigned int index = ( i * 4 + j ) * 4 + k;
				if ( i*i + j * j + k * k < 15 ) { 
					// include shell around 1 octant of sphere
					s2m[index] = m2s.size();
					m2s.push_back( index );
				} else {
					s2m[index] = ~0;
				}
			}
		}
	}
	unsigned int numEntries = m2s.size();
	cm.setMeshToSpace( m2s );
	cm.setSpaceToMesh( s2m );

	assert( cm.getNumEntries() == numEntries );
	assert( cm.getMeshEntrySize( 0 ) == 1 );

	coords = cm.getCoordinates( 0 );
	assert( doubleEq( coords[0], 1 ) );
	assert( doubleEq( coords[1], 2 ) );
	assert( doubleEq( coords[2], 4 ) );
	assert( doubleEq( coords[3], 2 ) );
	assert( doubleEq( coords[4], 3 ) );
	assert( doubleEq( coords[5], 5 ) );

	vector< unsigned int > neighbors = cm.getNeighbors( 0 );
	assert( neighbors.size() == 3 );
	assert( neighbors[0] = 1 );
	assert( neighbors[1] = s2m[ 4 ] );
	assert( neighbors[2] = s2m[ 16 ] );

	cout << "." << flush;
}


void testMesh()
{
	testCylMesh();
	testMidLevelCylMesh();
	testCubeMesh();
}
