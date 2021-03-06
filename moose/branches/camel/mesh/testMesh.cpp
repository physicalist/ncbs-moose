/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "SparseMatrix.h"
#include "../shell/Shell.h"
#include "Boundary.h"
#include "MeshEntry.h"
#include "Stencil.h"
#include "ChemMesh.h"
#include "CylMesh.h"
#include "CubeMesh.h"
#include "CylBase.h"
#include "NeuroNode.h"
#include "SparseMatrix.h"
#include "NeuroStencil.h"
#include "NeuroMesh.h"

/**
 * Low-level test for Cylbase, no MOOSE calls
 */
void testCylBase()
{
	// CylBase( x, y, z, dia, len, numDivs );
	CylBase  a( 0, 0, 0, 1,   1, 1 );
	CylBase  b( 1, 2, 3, 1,   2, 10 );

	assert( doubleEq( b.volume( a ), PI * 0.25 * 2 ) );
	assert( a.getNumDivs() == 1 );
	assert( b.getNumDivs() == 10 );

	for ( unsigned int i = 0; i < 10; ++i ) {
		assert( doubleEq( b.voxelVolume( a, i ), PI * 0.25 * 2 * 0.1 ) );
		vector< double > coords = b.getCoordinates( a, i );
		double x = i;
		assert( doubleEq( coords[0], x / 10.0 ) );
		assert( doubleEq( coords[1], x * 2.0 / 10.0 ) );
		assert( doubleEq( coords[2], x * 3.0  / 10.0 ) );
		assert( doubleEq( coords[3], (1.0 + x) / 10.0 ) );
		assert( doubleEq( coords[4], (1.0 + x) * 2.0 / 10.0 ) );
		assert( doubleEq( coords[5], (1.0 + x) * 3.0  / 10.0 ) );
		assert( doubleEq( coords[6], 0.5 ) );
		assert( doubleEq( coords[7], 0.5 ) );
		assert( doubleEq( coords[8], 0.0 ) );
		assert( doubleEq( coords[9], 0.0 ) );

		assert( doubleEq( b.getDiffusionArea( a, i ), PI * 0.25 ) );
	}
	b.setDia( 2.0 );
	assert( doubleEq( b.getDia(), 2.0 ) );
	assert( doubleEq( b.volume( a ), PI * (2*(0.5*0.5+0.5+1)/3.0) ) );
	for ( unsigned int i = 0; i < 10; ++i ) {
		double x = static_cast< double >( i ) / 10.0;
		double r0 = 0.5 * ( a.getDia() * ( 1.0 - x ) + b.getDia() * x );
		x = x + 0.1;
		double r1 = 0.5 * ( a.getDia() * ( 1.0 - x ) + b.getDia() * x );
		// len = 2, and we have 10 segments of it.
		double vv = 0.2 * ( r0 * r0 + r0 * r1 + r1 * r1 ) * PI / 3.0;
		assert( doubleEq( b.voxelVolume( a, i ), vv ) );
		assert( doubleEq( b.getDiffusionArea( a, i ), PI * r0 * r0 ) );
	}

	cout << "." << flush;
}

/**
 * Low-level test for NeuroNode, no MOOSE calls
 */
void testNeuroNode()
{
	CylBase  a( 0, 0, 0, 1,   1, 1 );
	CylBase  b( 1, 2, 3, 2,   2, 10 );
	CylBase  dcb( 1, 2, 3, 2,   2, 0 );
	vector< unsigned int > twoKids( 2, 2 );
	twoKids[0] = 2;
	twoKids[1] = 4;
	vector< unsigned int > oneKid( 1, 2 );
	vector< unsigned int > noKids( 0 );
	NeuroNode na( a, 0, twoKids, 0, Id(), true );
	NeuroNode ndummy( dcb, 0, oneKid, 1, Id(), false );
	NeuroNode nb( b, 1, noKids, 1, Id(), false );

	assert( na.parent() == 0 );
	assert( na.startFid() == 0 );
	assert( na.elecCompt() == Id() );
	assert( na.isDummyNode() == false );
	assert( na.isSphere() == true );
	assert( na.isStartNode() == true );
	assert( na.children().size() == 2 );
	assert( na.children()[0] == 2 );
	assert( na.children()[1] == 4 );
	assert( doubleEq( na.volume( a ), PI * 0.25 ) );

	assert( ndummy.parent() == 0 );
	assert( ndummy.startFid() == 1 );
	assert( ndummy.elecCompt() == Id() );
	assert( ndummy.isDummyNode() == true );
	assert( ndummy.isSphere() == false );
	assert( ndummy.isStartNode() == false );
	assert( ndummy.children().size() == 1 );
	assert( ndummy.children()[0] == 2 );

	assert( nb.parent() == 1 );
	assert( nb.startFid() == 1 );
	assert( nb.elecCompt() == Id() );
	assert( nb.isDummyNode() == false );
	assert( nb.isSphere() == false );
	assert( nb.isStartNode() == false );
	assert( nb.children().size() == 0 );
	assert( doubleEq( nb.volume( a ), PI * (2*(0.5*0.5+0.5+1)/3.0) ) );

	cout << "." << flush;
}

void zebraConcPattern( vector< vector< double > >& S )
{
	// Set up an alternating pattern of zero/nonzero voxels to do test
	// calculations
	assert( S.size() == 44 );
	for ( unsigned int i = 0; i < S.size(); ++i )
		S[i][0] = 0;

	S[0][0] = 100; // soma
	S[2][0] = 10; // basal dend
	S[4][0] = 1; // basal
	S[6][0] = 10; // basal
	S[8][0] = 1; // basal
	S[10][0] = 10; // basal
	S[12][0] = 1; // basal
	S[14][0] = 10; // basal
	S[16][0] = 1; // basal
	S[18][0] = 10; // basal
	S[20][0] = 1; // basal tip

	S[22][0] = 10; // Apical left dend second compt.
	S[24][0] = 1; // Apical left dend
	S[26][0] = 10; // Apical left dend
	S[28][0] = 1; // Apical left dend
	S[30][0] = 10; // Apical left dend last compt

	// Tip compts are 31 and 32:
	S[32][0] = 1; // Apical middle Tip compartment.

	S[34][0] = 10; // Apical right dend second compt.
	S[36][0] = 1; // Apical right dend
	S[38][0] = 10; // Apical right dend
	S[40][0] = 1; // Apical right dend
	S[42][0] = 10; // Apical right dend last compt

	S[43][0] = 1; // Apical right tip.
}

/// Set up uniform concentrations in each voxel. Only the end voxels
/// should see a nonzero flux.
void uniformConcPattern( vector< vector< double > >& S, 
				vector< double >& vs )
{
	// Set up uniform concs throughout. This means to scale each # by 
	// voxel size.
	assert( S.size() == 44 );
	for ( unsigned int i = 0; i < S.size(); ++i )
		S[i][0] = 10 * vs[i];
}

/**
 * Low-level test for NeuroStencil, no MOOSE calls
 * This outlines the policy for how nodes and dummies are 
 * organized.
 * The set of nodes represents a neuron like this:
 *
 *  \/ /  1
 *   \/   2
 *    |   3
 *    |   4
 * The soma is at the top of the vertical section. Diameter = 10 microns.
 * Each little segment is a NeuroNode. Assume 10 microns each.
 * Level 1 has only 1 voxel in each segment. 1 micron at tip
 * Level 2 has 10 voxels in each segment. 1 micron at tip,
 *   2 microns at soma surface
 * The soma is between levels 2 and 3 and is a single voxel.
 *  10 microns diameter.
 * Level 3 has 10 voxels. 3 micron at soma, 2 microns at tip
 * Level 4 has 10 voxels. 1 micron at tip.
 * We have a total of 8 full nodes, plus 3 dummy nodes for 
 * the segments at 2 and 3, since they plug into a sphere.
	Order is depth first: 
			0: soma
			1: basal dummy
			2: basal segment at level 3
			3: basal segment at level 4
			4: left dummy
			5: left segment at level 2, apicalL1
			6: left segment at level 1, apicalL2
			7: other (middle) segment at level 1;  apicalM
			8: right dummy
			9: right segment at level 2, apicalR1
			10: right segment at level 1, apicalR2
 */
unsigned int buildNode( vector< NeuroNode >& nodes, unsigned int parent, 
		double x, double y, double z, double dia, 
		unsigned int numDivs, bool isDummy, unsigned int startFid )
{
	x *= 1e-6;
	y *= 1e-6;
	z *= 1e-6;
	dia *= 1e-6;
	CylBase cb( x, y, z, dia, dia/2, numDivs );
	vector< unsigned int > kids( 0 );
	if ( nodes.size() == 0 ) { // make soma
		NeuroNode nn( cb, parent, kids, startFid, Id(), true );
		nodes.push_back( nn );
	} else if ( isDummy ) { // make dummy
		NeuroNode nn( cb, parent, kids, startFid, Id(), false );
		nodes.push_back( nn );
	} else {
		NeuroNode nn( cb, parent, kids, startFid, Id(), false );
		nodes.push_back( nn );
	}
	NeuroNode& paNode = nodes[ parent ];
	nodes.back().calculateLength( paNode );
	return startFid + numDivs;
}

void testNeuroStencilFlux()
{
	NeuroStencil ns;
	vector< double > flux( 1, 0 );
	vector< double > tminus( 1, 10.0 );
	vector< double > t0( 1, 100.0 );
	vector< double > tplus( 1, 1000.0 );
	vector< double > diffConst( 1, 1.0 );

	ns.addHalfFlux( 0, flux, t0, tminus, 1, 1, 1, 1, diffConst );
	assert( doubleEq( flux[0], -90 * NA ) );

	flux[0] = 0;
	ns.addLinearFlux( 0, flux, tminus, t0, tplus, 1, 1, 1, 1, 1, 1, diffConst );
	assert( doubleEq( flux[0], ( 1000 + 10 - 200 ) * NA ) );

	cout << "." << flush;
}

void connectChildNodes( vector< NeuroNode >& nodes )
{
	assert( nodes.size() == 11 );
	for ( unsigned int i = 1; i < nodes.size(); ++i ) {
		assert( nodes[i].parent() < nodes.size() );
		NeuroNode& parent = nodes[ nodes[i].parent() ];
		parent.addChild( i );
	}
	////////////////////////////////////////////////////////////////////
	// Check children
	assert( nodes[0].children().size() == 3 );
	assert( nodes[0].children()[0] == 1 );
	assert( nodes[0].children()[1] == 4 );
	assert( nodes[0].children()[2] == 8 );
	assert( nodes[1].children().size() == 1 );
	assert( nodes[1].children()[0] == 2 );
	assert( nodes[2].children().size() == 1 );
	assert( nodes[2].children()[0] == 3 );
	assert( nodes[3].children().size() == 0 );
	assert( nodes[4].children().size() == 1 );
	assert( nodes[4].children()[0] == 5 );
	assert( nodes[5].children().size() == 2 );
	assert( nodes[5].children()[0] == 6 );
	assert( nodes[5].children()[1] == 7 );
	assert( nodes[6].children().size() == 0 );
	assert( nodes[7].children().size() == 0 );
	assert( nodes[8].children().size() == 1 );
	assert( nodes[8].children()[0] == 9 );
	assert( nodes[9].children().size() == 1 );
	assert( nodes[9].children()[0] == 10 );
	assert( nodes[10].children().size() == 0 );
}


void testNeuroStencil()
{
	const unsigned int numNodes = 11;
	const unsigned int numVoxels = 44; // 4 with 10 each, soma, 3 branch end
	vector< NeuroNode > nodes;
	vector< unsigned int> nodeIndex( numVoxels, 0 );
	vector< double > vs( numVoxels, 1 );
	vector< double > area( numVoxels, 1 );
	double sq = 10.0 / sqrt( 2 );
		
	// CylBase( x, y, z, dia, len, numDivs )
	// NeuroNode( cb, parent, children, startFid, elecCompt, isSphere )
	// buildNode( nodes, pa, x, y, z, dia, numDivs, isDummy, startFid )
	unsigned int startFid = 0;
	////////////////////////////////////////////////////////////////////
	// Build the nodes
	////////////////////////////////////////////////////////////////////
	startFid = buildNode( nodes, 0, 0, 0, 0, 10, 1, 0, startFid ); // soma
	startFid = buildNode( nodes, 0, 0, -5, 0, 3, 0, 1, startFid ); // dummy1
	startFid = buildNode( nodes, 1, 0, -15, 0, 2, 10, 0, startFid ); // B1
	startFid = buildNode( nodes, 2, 0, -25, 0, 1, 10, 0, startFid ); // B2

	startFid = buildNode( nodes, 0, 0, 5, 0, 2, 0, 1, startFid ); // dummy2
	startFid = buildNode( nodes, 4, -sq, 5+sq, 0, 1, 10, 0, startFid ); // AL1
	startFid = buildNode( nodes, 5, -sq*2, 5+sq*2, 0, 1, 1, 0, startFid ); // AL2
	startFid = buildNode( nodes, 5, 0, 5+sq*2, 0, 1, 1, 0, startFid ); // AM
	startFid = buildNode( nodes, 0, 0, 5, 0, 2, 0, 1, startFid ); // dummy3
	startFid = buildNode( nodes, 8, sq, 5+sq, 0, 1, 10, 0, startFid ); //AR1
	startFid = buildNode( nodes, 9, sq*2, 5+sq*2, 0, 1, 1, 0, startFid ); //AR2
	assert( startFid == numVoxels );
	assert( nodes.size() == numNodes );
	connectChildNodes( nodes );

	////////////////////////////////////////////////////////////////////
	// Check lengths
	for ( unsigned int i = 0; i < numNodes; ++i ) {
		if ( !( nodes[i].isDummyNode() || nodes[i].isSphere() ) )
			assert( doubleEq( nodes[i].getLength(), 10e-6 ) );
	}
	////////////////////////////////////////////////////////////////////
	// Build nodeIndex vs and area arrays.
	for ( unsigned int i = 0; i < numNodes; ++i ) {
		assert( nodes[i].parent() < numNodes );
		NeuroNode& parent = nodes[ nodes[i].parent() ];
		unsigned int end = nodes[i].startFid() + nodes[i].getNumDivs();
		for ( unsigned int j = nodes[i].startFid(); j < end; ++j ) {
			assert( j < numVoxels );
			nodeIndex[j] = i;
			unsigned int k = j - nodes[i].startFid();
			vs[j] = 0.001 * NA * nodes[i].voxelVolume( parent, k );
			area[j] = nodes[i].getDiffusionArea( parent, k );
		}
	}
	assert( doubleEq( vs[ numVoxels-1 ], NA * 0.001 * PI * 10e-6 * 0.25e-12 ) );
	assert( doubleEq( area[ numVoxels-1 ], PI * 0.25e-12 ) );
	////////////////////////////////////////////////////////////////////
	// Setup done. Now build stencil and test it.
	////////////////////////////////////////////////////////////////////
	vector< double > diffConst( 1, 1e-12 );
	vector< double > temp( 1, 0.0 );
	vector< vector< double > > flux( numVoxels, temp );
	vector< double > molNum( 1, 0 ); // We only have one pool

	// S[meshIndex][poolIndex]
	vector< vector< double > > S( numVoxels, molNum ); 

	NeuroStencil ns( nodes, nodeIndex, vs, area );

	zebraConcPattern( S );

	for ( unsigned int i = 0; i < numVoxels; ++i ) {
		ns.addFlux( i, flux[i], S, diffConst );
	}
	/*
	for ( unsigned int i = 0; i < numVoxels; ++i ) {
		cout << "S[" << i << "][0] = " << S[i][0] << 
			", flux[" << i << "][0] = " << flux[i][0] << endl;
	}
	*/

	uniformConcPattern( S, vs );

	for ( unsigned int i = 0; i < numVoxels; ++i ) {
		flux[i][0] = 0.0;
		ns.addFlux( i, flux[i], S, diffConst );
	}
	for ( unsigned int i = 0; i < numVoxels; ++i ) {
		assert( doubleEq( flux[i][0] / 1e12, 0.0 ) );
		// Actually we could divide flux by NA: the roundoff error is
		// due to differences in NA-type numbers.
		/*
		cout << "S[" << i << "][0] = " << S[i][0] << 
			", flux[" << i << "][0] = " << flux[i][0] << endl;
			*/
	}
	
	cout << "." << flush;
}

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

	cm.innerSetCoords( coords );

	assert( doubleEq( cm.getX0(), 1 ) );
	assert( doubleEq( cm.getY0(), 2 ) );
	assert( doubleEq( cm.getZ0(), 3 ) );
	assert( doubleEq( cm.getR0(), 1 ) );

	assert( doubleEq( cm.getX1(), 3 ) );
	assert( doubleEq( cm.getY1(), 5 ) );
	assert( doubleEq( cm.getZ1(), 7 ) );
	assert( doubleEq( cm.getR1(), 2 ) );

	assert( doubleEq( cm.getLambda(), sqrt( 29.0 ) / 5.0 ) );

	cm.setX0( 2 );
	cm.setY0( 3 );
	cm.setZ0( 4 );
	cm.setR0( 2 );

	cm.setX1( 4 );
	cm.setY1( 6 );
	cm.setZ1( 8 );
	cm.setR1( 3 );

	cm.setLambda( 2.0 );

	vector< double > temp = cm.getCoords( Id().eref(), 0 );
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
	vector< int > dims( 1, 1 );

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
	meshId.element()->syncFieldDim();

	assert( meshId()->dataHandler()->localEntries() == 5 );

	unsigned int n = Field< unsigned int >::get( cylId, "num_mesh" );
	assert( n == 5 );

	ObjId oid( meshId, DataId( 2 ) );

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
	cm.setPreserveNumEntries( 0 );
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

	cm.innerSetCoords( coords );

	vector< unsigned int > neighbors = cm.getNeighbors( 0 );
	assert( neighbors.size() == 3 );
	assert( neighbors[0] = 1 );
	assert( neighbors[0] = 2 );
	assert( neighbors[0] = 8 );

	assert( cm.innerGetNumEntries() == 64 );
	assert( doubleEq( cm.getX0(), 0 ) );
	assert( doubleEq( cm.getY0(), 0 ) );
	assert( doubleEq( cm.getZ0(), 0 ) );

	assert( doubleEq( cm.getX1(), 2 ) );
	assert( doubleEq( cm.getY1(), 4 ) );
	assert( doubleEq( cm.getZ1(), 8 ) );

	assert( doubleEq( cm.getDx(), 1 ) );
	assert( doubleEq( cm.getDy(), 1 ) );
	assert( doubleEq( cm.getDz(), 1 ) );

	assert( cm.getNx() == 2 );
	assert( cm.getNy() == 4 );
	assert( cm.getNz() == 8 );

	cm.setX0( 1 );
	cm.setY0( 2 );
	cm.setZ0( 4 );

	cm.setX1( 5 );
	cm.setY1( 6 );
	cm.setZ1( 8 );

	vector< double > temp = cm.getCoords( Id().eref(), 0 );
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
	assert( cm.innerGetNumEntries() == 64 );
	assert( cm.getNx() == 4 );
	assert( cm.getNy() == 4 );
	assert( cm.getNz() == 4 );

	neighbors = cm.getNeighbors( 0 );
	assert( neighbors.size() == 3 );
	assert( neighbors[0] == 1 );
	assert( neighbors[1] == 4 );
	assert( neighbors[2] = 16 );

	neighbors = cm.getNeighbors( 63 );
	assert( neighbors.size() == 3 );
	assert( neighbors[0] == 47 );
	assert( neighbors[1] == 59 );
	assert( neighbors[2] == 62 );

	neighbors = cm.getNeighbors( 2 );
	assert( neighbors.size() == 4 );
	assert( neighbors[0] == 1 );
	assert( neighbors[1] == 3 );
	assert( neighbors[2] == 6 );
	assert( neighbors[3] == 18 );

	neighbors = cm.getNeighbors( 6 );
	assert( neighbors.size() == 5 );
	assert( neighbors[0] == 2 );
	assert( neighbors[1] == 5 );
	assert( neighbors[2] == 7 );
	assert( neighbors[3] == 10 );
	assert( neighbors[4] == 22 );

	neighbors = cm.getNeighbors( 22 );
	assert( neighbors.size() == 6 );
	assert( neighbors[0] == 6 );
	assert( neighbors[1] == 18 );
	assert( neighbors[2] == 21 );
	assert( neighbors[3] == 23 );
	assert( neighbors[4] == 26 );
	assert( neighbors[5] == 38 );

	cm.setPreserveNumEntries( 1 );
	assert( cm.getNx() == 4 );
	assert( cm.getNy() == 4 );
	assert( cm.getNz() == 4 );
	assert( doubleEq( cm.getDx(), 1.0 ) );
	assert( doubleEq( cm.getDy(), 1.0 ) );
	assert( doubleEq( cm.getDz(), 1.0 ) );

	cm.setX0( 0 );
	cm.setY0( 0 );
	cm.setZ0( 0 );
	// x1 is 5, y1 is 6 and z1 is 8

	assert( doubleEq( cm.getDx(), 1.25 ) );
	assert( doubleEq( cm.getDy(), 1.5 ) );
	assert( doubleEq( cm.getDz(), 2.0 ) );

	cout << "." << flush;
}

void testCubeMeshExtendStencil()
{
	CubeMesh cm0;
	cm0.setPreserveNumEntries( 0 );
	assert( cm0.getMeshType( 0 ) == CUBOID );
	assert( cm0.getMeshDimensions( 0 ) == 3 );
	assert( cm0.getDimensions() == 3 );
	CubeMesh cm1 = cm0;

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

	cm0.innerSetCoords( coords );
	coords[2] = 8; // 2x4 face abuts.
	coords[5] = 16;
	cm1.innerSetCoords( coords );

	const double* entry;
	const unsigned int* colIndex;
	unsigned int num = cm0.getStencil( 0, &entry, &colIndex );
	assert( num == 3 );
	assert( colIndex[0] == 1 );
	assert( colIndex[1] == 2 );
	assert( colIndex[2] == 8 );

	num = cm0.getStencil( 56, &entry, &colIndex );
	assert( num == 3 );
	assert( colIndex[0] == 48 );
	assert( colIndex[1] == 57 );
	assert( colIndex[2] == 58 );

	vector< VoxelJunction > vj;
	for ( unsigned int i = 0; i < 8; ++i ) {
		vj.push_back( VoxelJunction( 56 + i, i ) );
	}
	cm0.extendStencil( &cm1, vj );

	num = cm0.getStencil( 56, &entry, &colIndex );
	assert( num == 4 );
	assert( colIndex[0] == 48 );
	assert( colIndex[1] == 57 );
	assert( colIndex[2] == 58 );
	assert( colIndex[3] == 64 );

	for ( unsigned int i = 0; i < 8; ++i ) {
		num = cm0.getStencil( 64 + i, &entry, &colIndex );
		assert( num == 1 );
		assert( colIndex[0] == 56 + i );
	}

	cout << "." << flush;
}

/**
 * Tests scaling of volume and # of entries upon mesh resize
 */
void testReMesh()
{
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	vector< int > dims( 1, 1 );
	Id base = s->doCreate( "Neutral", Id(), "base", dims );

	Id pool = s->doCreate( "Pool", base, "pool", dims );
	Id cube = s->doCreate( "CubeMesh", base, "cube", dims );
	bool ret = SetGet2< double, unsigned int >::set( 
		cube, "buildDefaultMesh", 1.0, 1 );
	assert( ret );
	Id mesh( "/base/cube/mesh" );
	assert( mesh != Id() );

	MsgId mid = s->doAddMsg( "OneToOne", pool, "mesh", mesh, "mesh" );
	assert( mid != Msg::bad );

/////////////////////////////////////////////////////////////////
	// 1 millimolar in 1 m^3 is 1 mole per liter.
	unsigned int linsize = Field< unsigned int >::get( pool, "linearSize" );
	assert( linsize == 1 );

	ret = Field< double >::set( pool, "conc", 1 );
	assert( ret );
	double n = Field< double >::get( pool, "n" );
	assert( doubleEq( n, NA ) );

	ret = SetGet2< double, unsigned int >::set( 
		cube, "buildDefaultMesh", 1.0e-3, 1 );
	Field< double >::set( pool, "conc", 1 );
	n = Field< double >::get( pool, "n" );
	assert( doubleEq( n, NA / 1000.0 ) );

/////////////////////////////////////////////////////////////////
	// Next we do the remeshing.
	double x = 1.234;
	Field< double >::set( pool, "concInit", x );
	ret = SetGet2< double, unsigned int >::set( 
		cube, "buildDefaultMesh", 1, 8 );
	// This is nasty, needs the change to propagate through messages.
	// Qinfo::waitProcCycles( 2 );
	linsize = Field< unsigned int >::get( pool, "linearSize" );
	assert( linsize == 8 );

	n = Field< double >::get( ObjId( pool, 0 ), "concInit" );
	assert( doubleEq( n, x ) );
	n = Field< double >::get( ObjId( pool, 7 ), "concInit" );
	assert( doubleEq( n, x ) );
	n = Field< double >::get( ObjId( pool, 0 ), "nInit" );
	assert( doubleEq( n, x * NA / 8.0 ) );
	n = Field< double >::get( ObjId( pool, 7 ), "nInit" );
	assert( doubleEq( n, x * NA / 8.0 ) );
	n = Field< double >::get( ObjId( pool, 0 ), "conc" );
	assert( doubleEq( n, x ) );
	n = Field< double >::get( ObjId( pool, 7 ), "conc" );
	assert( doubleEq( n, x ) );

/////////////////////////////////////////////////////////////////
	s->doDelete( base );
	cout << "." << flush;
}

/**
 * theta in degrees
 * len and dia in metres as usual
 */
Id makeCompt( Id parentCompt, Id parentObj, 
		string name, double len, double dia, double theta )
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	vector< int > dims( 1, 1 );
	Id ret = shell->doCreate( "Compartment", parentObj, name, dims );
	double pax = 0;
	double pay = 0;
	if ( parentCompt != Id() ) {
		pax = Field< double >::get( parentCompt, "x" );
		pay = Field< double >::get( parentCompt, "y" );
		shell->doAddMsg( "Single", parentCompt, "raxial", ret, "axial" );
	}
	Field< double >::set( ret, "x0", pax );
	Field< double >::set( ret, "y0", pay );
	Field< double >::set( ret, "z0", 0.0 );
	double x = pax + len * sin( theta * PI / 360.0 );
	double y = pay + len * cos( theta * PI / 360.0 );
	Field< double >::set( ret, "x", x );
	Field< double >::set( ret, "y", y );
	Field< double >::set( ret, "y", 0.0 );
	Field< double >::set( ret, "diameter", dia );
	Field< double >::set( ret, "length", len );

	return ret;
}

pair< unsigned int, unsigned int > buildBranchingCell( 
				Id cell, double len, double dia )
{
	Id soma = makeCompt( Id(), cell, "soma", dia, dia, 0 );
	dia /= sqrt( 2.0 );
	Id d1 = makeCompt( soma, cell, "d1", len , dia, 0 );
	Id d2 = makeCompt( soma, cell, "d2", len , dia, 180 );
	Id d1a = makeCompt( d1, cell, "d1a", len , dia, 0 );
	Id d2a = makeCompt( d2, cell, "d2a", len , dia, 180 );
	dia /= sqrt( 2.0 );
	Id d11 = makeCompt( d1a, cell, "d11", len , dia, -45 );
	Id d12 = makeCompt( d1a, cell, "d12", len , dia, 45 );
	Id d21 = makeCompt( d2a, cell, "d21", len , dia, 45 );
	Id d22 = makeCompt( d2a, cell, "d22", len , dia, -45 );
	dia /= sqrt( 2.0 );
	Id d111 = makeCompt( d11, cell, "d111", len , dia, -90 );
	Id d112 = makeCompt( d11, cell, "d112", len , dia, 0 );
	Id d121 = makeCompt( d12, cell, "d121", len , dia, 0 );
	Id d122 = makeCompt( d12, cell, "d122", len , dia, 90 );
	Id d211 = makeCompt( d21, cell, "d211", len , dia, 90 );
	Id d212 = makeCompt( d21, cell, "d212", len , dia, 180 );
	Id d221 = makeCompt( d22, cell, "d221", len , dia, 180 );
	Id d222 = makeCompt( d22, cell, "d222", len , dia, -90 );

	return pair< unsigned int, unsigned int >( 17, 161 );
}

	// y = initConc * dx * (0.5 / sqrt( PI * DiffConst * runtime ) ) * 
	//        exp( -x * x / ( 4 * DiffConst * runtime ) )
double diffusionFunction( double D, double dx, double x, double t )
{
	return
		dx * (0.5 / sqrt( PI * D * t ) ) * exp( -x * x / ( 4 * D * t ) );
}

void testNeuroMeshLinear()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	vector< int > dims( 1, 1 );
	// Build a linear cylindrical cell, no tapering.
	Id cell = shell->doCreate( "Neutral", Id(), "cell", dims );
	unsigned int numCompts = 500;
	double dia = 1e-6; // metres
	double diffLength = 0.2e-6; // metres
	double len = diffLength * numCompts;
	double maxt = 100.0;
	double dt = 0.01;
	double D = 1e-12;
	double totNum = 1e6;

	Id soma = makeCompt( Id(), cell, "soma", len, dia, 0 );

	// Scan it with neuroMesh and check outcome.
	Id nm = shell->doCreate( "NeuroMesh", Id(), "neuromesh", dims );
	Field< double >::set( nm, "diffLength", diffLength );
	Field< string >::set( nm, "geometryPolicy", "cylinder" );
	Field< Id >::set( nm, "cell", cell );
	unsigned int ns = Field< unsigned int >::get( nm, "numSegments" );
	assert( ns == 1 );
	unsigned int ndc = Field< unsigned int >::get( nm, "numDiffCompts" );
	assert( ndc == numCompts );
	const vector< NeuroNode >& nodes = 
			reinterpret_cast< NeuroMesh* >( nm.eref().data() )->
			getNodes();
	assert( nodes.size() == 1 );
	assert( nodes[0].children().size() == 0 );

	// Insert a molecule at first subdivision of soma. I use a dummy 
	// matrix S rather than the one in the system.
	// Field< double >::set( ObjId( soma, 0 ), "nInit", 1.0e6 );
	vector< double > molNum( 1, 0 ); // We only have one pool
	// S[meshIndex][poolIndex]
	vector< vector< double > > S( ndc, molNum ); 
	S[0][0] = totNum;
	vector< double > diffConst( 1, D );
	vector< double > temp( 1, 0.0 );
	vector< vector< double > > flux( ndc, temp );
	
	// Watch diffusion using stencil and direct calls to the flux 
	// calculations rather than going through the ksolve.
	const Stencil* stencil = 
			reinterpret_cast< NeuroMesh* >( nm.eref().data() )->
			getStencil();
	assert( stencil != 0 );	
	for ( double t = 0; t < maxt; t += dt ) {
		for ( unsigned int i = 0; i < ndc; ++i )
			flux[i][0] = 0.0;
		
		for ( unsigned int i = 0; i < ndc; ++i ) {
			stencil->addFlux( i, flux[i], S, diffConst );
		}
		for ( unsigned int i = 0; i < ndc; ++i )
			S[i][0] += flux[i][0] * dt;
	}

	// Compare with analytic calculation
	double tot1 = 0.0;
	double tot2 = 0.0;
	double dlBy2 = diffLength/2.0;
	double x = dlBy2;
	for ( unsigned int j = 0; j < numCompts; ++j ) {
			// Factor of two because we compute a half-infinite cylinder.
		double y = 2 * totNum * 
			diffusionFunction( diffConst[0], diffLength, x, maxt );
		unsigned int k = j + nodes[0].startFid();
		//cout << "S[" << k << "][0] = " << S[k][0] << "	" << y << endl;
		tot1 += S[k][0];
		tot2 += y;
		x += diffLength;
		// Here we compare only half of the length because edge effects 
		// mess up the flux calculation.
		if ( j < numCompts/2 )
			assert ( doubleApprox( y, S[k][0] ) ); 
	}
	assert( doubleEq( tot1, totNum ) );
	assert( doubleEq( tot2, totNum ) );
	
	shell->doDelete( cell );
	shell->doDelete( nm );
	cout << "." << flush;
}

void testNeuroMeshBranching()
{
	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	vector< int > dims( 1, 1 );
	// Build a cell
	Id cell = shell->doCreate( "Neutral", Id(), "cell", dims );
	double len = 10e-6; // metres
	double dia = 1e-6; // metres
	double diffLength = 1e-6; // metres
	double D = 4e-12; // Diff const, m^2/s
	double totNum = 1e6; // Molecules
	double maxt = 10.0;
	double dt = 0.001;

	pair< unsigned int, unsigned int > ret = 
			buildBranchingCell( cell, len, dia );

	// Scan it with neuroMesh and check outcome.
	Id nm = shell->doCreate( "NeuroMesh", Id(), "neuromesh", dims );
	Field< double >::set( nm, "diffLength", diffLength );
	Field< string >::set( nm, "geometryPolicy", "cylinder" );
	Field< Id >::set( nm, "cell", cell );
	unsigned int ns = Field< unsigned int >::get( nm, "numSegments" );
	assert( ns == ret.first );
	unsigned int ndc = Field< unsigned int >::get( nm, "numDiffCompts" );
	assert( ndc == ret.second );
	const vector< NeuroNode >& nodes = 
			reinterpret_cast< NeuroMesh* >( nm.eref().data() )->
			getNodes();
	assert( nodes.size() == ns + 14 ); // 14 dummy nodes.
	assert( nodes[0].children().size() == 2 );
	assert( nodes[1].children().size() == 1 );
	assert( nodes[2].children().size() == 1 );
	assert( nodes[3].children().size() == 2 );
	assert( nodes[4].children().size() == 2 );
	assert( nodes[5].children().size() == 2 );
	assert( nodes[6].children().size() == 2 );


	// Insert a molecule at soma
	vector< double > molNum( 1, 0 ); // We only have one pool
	// S[meshIndex][poolIndex]
	vector< vector< double > > S( ndc, molNum ); 
	S[0][0] = totNum;
	vector< double > diffConst( 1, D );
	vector< double > temp( 1, 0.0 );
	vector< vector< double > > flux( ndc, temp );
	
	// Watch diffusion using stencil and direct calls to the flux 
	// calculations rather than going through the ksolve.
	const Stencil* stencil = 
			reinterpret_cast< NeuroMesh* >( nm.eref().data() )->
			getStencil();
	assert( stencil != 0 );	
	for ( double t = 0; t < maxt; t += dt ) {
		for ( unsigned int i = 0; i < ndc; ++i )
			flux[i][0] = 0.0;
		
		for ( unsigned int i = 0; i < ndc; ++i ) {
			stencil->addFlux( i, flux[i], S, diffConst );
		}
		for ( unsigned int i = 0; i < ndc; ++i )
			S[i][0] += flux[i][0] * dt;
	}
	double tot = 0.0;
	for ( unsigned int i = 0; i < nodes.size(); ++i ) {
		for ( unsigned int j = 0; j < nodes[i].getNumDivs(); ++j ) {
			unsigned int k = j + nodes[i].startFid();
			//cout << "S[" << k << "][0] = " << S[k][0] << endl;
			tot += S[k][0];
		}
	}

	// Compare with analytic solution.
	assert( doubleEq( tot, totNum ) );
	double x = 1.5 * diffLength;
	for ( unsigned int i = 1; i < 11; ++i ) { // First segment of tree.
		double y = totNum * diffusionFunction(
				diffConst[0], diffLength, x, maxt );
		assert( doubleApprox( y, S[i][0] ) );
		// cout << "S[" << i << "][0] = " << S[i][0] << "	" << y << endl;
		x += diffLength;
	}
	tot = 0;
	for ( double x = diffLength / 2.0 ; x < 10 * len; x += diffLength ) {
			// the 2x is needed because we add up only half of the 2-sided
			// diffusion distribution.
		double y = 2 * totNum * diffusionFunction( 
				diffConst[0], diffLength, x, maxt );
		// cout << floor( x / diffLength ) << ": " << y << endl;
		tot += y;
	}
	assert( doubleEq( tot, totNum ) );
	
	shell->doDelete( cell );
	shell->doDelete( nm );
	cout << "." << flush;
}

// Assorted definitions from CubeMesh.cpp
static const unsigned int EMPTY = ~0;
static const unsigned int SURFACE = ~1;
static const unsigned int ABUT = ~2;
static const unsigned int MULTI = ~3;
typedef pair< unsigned int, unsigned int > PII;
extern void setIntersectVoxel( 
		vector< PII >& intersect, 
		unsigned int ix, unsigned int iy, unsigned int iz,
		unsigned int nx, unsigned int ny, unsigned int nz,
		unsigned int meshIndex );

extern void checkAbut( 
		const vector< PII >& intersect, 
		unsigned int ix, unsigned int iy, unsigned int iz,
		unsigned int nx, unsigned int ny, unsigned int nz,
		unsigned int meshIndex,
		vector< VoxelJunction >& ret );

void testIntersectVoxel()
{
		/**
		 * Here is the geometry of the surface. * is surface, - is empty.
		 *
		 *			-***-
		 *			-*---
		 *			-***-
		 *
		 *
		 *			x***x
		 *			x*32-
		 *			x***x
		 *
		 * 	x is ABUTX
		 * 	y is ABUTY
		 * 	z is ABUTZ
		 * 	2 is 2 points
		 * 	3 is MULTI
		 *
		 */


	unsigned int nx = 5;
	unsigned int ny = 3;
	unsigned int nz = 1;
	vector< PII > intersect( nx * ny * nz, PII( 
							CubeMesh::EMPTY, CubeMesh::EMPTY ) );
	unsigned int meshIndex = 0;
	setIntersectVoxel( intersect, 1, 0, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 2, 0, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 3, 0, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 1, 1, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 1, 2, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 2, 2, 0, nx, ny, nz, meshIndex++ );
	setIntersectVoxel( intersect, 3, 2, 0, nx, ny, nz, meshIndex++ );

	assert( intersect[0].first == 0 && 
					intersect[0].second == CubeMesh::ABUTX );
	assert( intersect[1].first == 0 && 
					intersect[1].second == CubeMesh::SURFACE );
	assert( intersect[2].first == 1 && 
					intersect[2].second == CubeMesh::SURFACE );
	assert( intersect[3].first == 2 && 
					intersect[3].second == CubeMesh::SURFACE );
	assert( intersect[4].first == 2 && 
					intersect[4].second == CubeMesh::ABUTX );

	assert( intersect[5].first == 3 && 
					intersect[5].second == CubeMesh::ABUTX );
	assert( intersect[6].first == 3 && 
					intersect[6].second == CubeMesh::SURFACE );
	assert( intersect[7].first == 1 && 
					intersect[7].second == CubeMesh::MULTI );
	assert( intersect[8].first == 2 && 
					intersect[8].second == CubeMesh::MULTI );
	assert( intersect[9].first == EMPTY && 
					intersect[9].second == CubeMesh::EMPTY );

	assert( intersect[10].first == 4 && 
					intersect[10].second == CubeMesh::ABUTX );
	assert( intersect[11].first == 4 && 
					intersect[11].second == CubeMesh::SURFACE );
	assert( intersect[12].first == 5 && 
					intersect[12].second == CubeMesh::SURFACE );
	assert( intersect[13].first == 6 && 
					intersect[13].second == CubeMesh::SURFACE );
	assert( intersect[14].first == 6 && 
					intersect[14].second == CubeMesh::ABUTX );

	// Next: test out checkAbut.
	vector< VoxelJunction > ret;
	checkAbut( intersect, 0, 0, 0, nx, ny, nz, 1234, ret );
	assert( ret.size() == 1 );
	assert( ret[0].first == 0 && ret[0].second == 1234 );
	ret.clear();
	// The ones below are either SURFACE or EMPTY and should not add points
	checkAbut( intersect, 1, 0, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 2, 0, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 3, 0, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 1, 1, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 4, 1, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 1, 2, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 2, 2, 0, nx, ny, nz, 1234, ret );
	checkAbut( intersect, 3, 2, 0, nx, ny, nz, 1234, ret );
	assert( ret.size() == 0 );
	checkAbut( intersect, 2, 1, 0, nx, ny, nz, 9999, ret );
	assert( ret.size() == 3 );
	assert( ret[0].first == 3 && ret[0].second == 9999 );
	assert( ret[1].first == 1 && ret[1].second == 9999 );
	assert( ret[2].first == 5 && ret[1].second == 9999 );
	ret.clear();
	checkAbut( intersect, 3, 1, 0, nx, ny, nz, 8888, ret );
	assert( ret.size() == 2 );
	assert( ret[0].first == 2 && ret[0].second == 8888 );
	assert( ret[1].first == 6 && ret[1].second == 8888 );
	ret.clear();
	checkAbut( intersect, 4, 0, 0, nx, ny, nz, 7777, ret );
	checkAbut( intersect, 0, 1, 0, nx, ny, nz, 6666, ret );
	checkAbut( intersect, 0, 2, 0, nx, ny, nz, 5555, ret );
	checkAbut( intersect, 4, 2, 0, nx, ny, nz, 4444, ret );
	assert( ret.size() == 4 );
	assert( ret[0].first == 2 && ret[0].second == 7777 );
	assert( ret[1].first == 3 && ret[1].second == 6666 );
	assert( ret[2].first == 4 && ret[2].second == 5555 );
	assert( ret[3].first == 6 && ret[3].second == 4444 );
	
	cout << "." << flush;
}

void testCubeMeshFillTwoDimSurface()
{
	CubeMesh cm;
	vector< double > coords( 9, 0.0 );
	coords[3] = 5.0;
	coords[4] = 3.0;
	coords[5] = 1.0;
	coords[6] = coords[7] = coords[8] = 1.0;
	cm.setPreserveNumEntries( false );
	cm.innerSetCoords( coords );
	assert( cm.numDims() == 2 );
	const vector< unsigned int >& surface = cm.surface();
	assert( surface.size() == 12 );
	for ( unsigned int i = 0; i < 5; ++i ) {
		assert( surface[i] == i );
		assert( surface[i + 5] == i + 10 );
	}
	assert( surface[10] == 5 );
	assert( surface[11] == 9 );
	cout << "." << flush;
}

void testCubeMeshFillThreeDimSurface()
{
	cout << "." << flush;
}

void testCubeMeshJunctionTwoDimSurface()
{
		/**					
		 * 						8	9
		 * 10	11	12	13	14	6	7
		 * 5	6	7	8	9	4	5
		 * 0	1	2	3	4	2	3
		 * 						0	1
		 *
		 * So, junction should be (4,2),(9,4),(14,6)
		 */
	CubeMesh cm1;
	vector< double > coords( 9, 0.0 );
	coords[3] = 5.0;
	coords[4] = 3.0;
	coords[5] = 1.0;
	coords[6] = coords[7] = coords[8] = 1.0;
	cm1.setPreserveNumEntries( false );
	cm1.innerSetCoords( coords );
	vector< unsigned int > surface = cm1.surface();
	assert( surface.size() == 12 );

	CubeMesh cm2;
	coords[0] = 5.0;
	coords[1] = -1.0;
	coords[2] = 0.0;
	coords[3] = 7.0;
	coords[4] = 4.0;
	coords[5] = 1.0;
	coords[6] = coords[7] = coords[8] = 1.0;
	cm2.setPreserveNumEntries( false );
	cm2.innerSetCoords( coords );
	const vector< unsigned int >& surface2 = cm2.surface();
	assert( surface2.size() == 10 );

	vector< VoxelJunction > ret;
	cm1.matchCubeMeshEntries( &cm2, ret );
	assert( ret.size() == 3 ); 

	assert( ret[0].first == 4 );
	assert( ret[0].second == 2 );
	assert( ret[1].first == 9 );
	assert( ret[1].second == 4 );
	assert( ret[2].first == 14 );
	assert( ret[2].second == 6 );

	/**
	 * That was too easy, since the spatial and meshIndices were
	 * identical. Now trim the geometries a bit to look like:
		 * 						6	7
		 * 10	11	12	13	-	4	5
		 * 5	6	7	8	9	2	3
		 * 0	1	2	3	4	-	1
		 * 						-	0
		 *
		 * So, junction should be (9,2) only.
		 */
	
	// Trimming cm1. At this point we don't assume automatic updates of
	// the m2s, s2m and surface vectors when any of them is changed.
	vector< unsigned int > m2s = cm1.getMeshToSpace();
	assert( m2s.size() == 15 );
	m2s.resize( 14 );
	cm1.setMeshToSpace( m2s );
	vector< unsigned int > s2m = cm1.getSpaceToMesh();
	assert( s2m.size() == 15 );
	s2m[14] = ~0;
	cm1.setSpaceToMesh( s2m );
	surface.resize( 4 ); 
	// As a shortcut, just assign the places near the junction
	// Note that the indices are spaceIndices.
	surface[0] = 3;
	surface[1] = 4;
	surface[2] = 9;
	surface[3] = 13;
	cm1.setSurface( surface );
	
	// Trimming cm2.
	m2s = cm2.getMeshToSpace();
	assert( m2s.size() == 10 );
	m2s.resize( 8 );
	m2s[0] = 1;
	for ( unsigned int i = 1; i < 8; ++i )
		m2s[i] = i + 2;
	cm2.setMeshToSpace( m2s );
	s2m.clear();
	s2m.resize( 10, ~0 );
	for ( unsigned int i = 0; i < 8; ++i )
		s2m[ m2s[i] ] = i;
	cm2.setSpaceToMesh( s2m );
	// As a shortcut, just assign the places near the junction
	// Note that the indices are spaceIndices.
	surface[0] = 3;
	surface[1] = 4;
	surface[2] = 6;
	surface[3] = 8;
	cm2.setSurface( surface );

	// Now test it out.
	ret.resize( 0 );
	cm1.matchCubeMeshEntries( &cm2, ret );
	assert( ret.size() == 1 ); 
	assert( ret[0].first == 9 );
	assert( ret[0].second == 2 );

	cout << "." << flush;
}

void testCubeMeshJunctionDiffSizeMesh()
{
		/**					
		 * 						14	15
		 * 						12	13
		 * 10	11	12	13	14	10	11
		 *						8	9
		 * 5	6	7	8	9	6	7
		 *						4	5
		 * 0	1	2	3	4	2	3
		 * 						0	1
		 *
		 * So, junction should be (4,2)(4,4),(9,6),(9,8),(14,10),(14,12)
		 */
	CubeMesh cm1;
	vector< double > coords( 9, 0.0 );
	coords[3] = 5.0;
	coords[4] = 3.0;
	coords[5] = 1.0;
	coords[6] = coords[7] = coords[8] = 1.0;
	cm1.setPreserveNumEntries( false );
	cm1.innerSetCoords( coords );
	vector< unsigned int > surface = cm1.surface();
	assert( surface.size() == 12 );

	CubeMesh cm2;
	coords[0] = 5.0;
	coords[1] = -0.5;
	coords[2] = 0.0;
	coords[3] = 7.0;
	coords[4] = 3.5;
	coords[5] = 0.5;
	coords[6] = 1.0;
	coords[7] = 0.5;
   	coords[8] = 0.5;
	cm2.setPreserveNumEntries( false );
	cm2.innerSetCoords( coords );
	const vector< unsigned int >& surface2 = cm2.surface();
	assert( surface2.size() == 16 );

	vector< VoxelJunction > ret;
	cm1.matchCubeMeshEntries( &cm2, ret );
	assert( ret.size() == 6 ); 

	assert( ret[0].first == 4 );
	assert( ret[0].second == 2 );
	assert( ret[1].first == 4 );
	assert( ret[1].second == 4 );
	assert( ret[2].first == 9 );
	assert( ret[2].second == 6 );
	assert( ret[3].first == 9 );
	assert( ret[3].second == 8 );
	assert( ret[4].first == 14 );
	assert( ret[4].second == 10 );
	assert( ret[5].first == 14 );
	assert( ret[5].second == 12 );

	cout << "." << flush;
}

void testCubeMeshJunctionThreeDimSurface()
{
	cout << "." << flush;
}

void testMesh()
{
	testCylBase();
	testNeuroNode();
	testNeuroStencilFlux();
	testNeuroStencil();
	testCylMesh();
	testMidLevelCylMesh();
	testCubeMesh();
	testCubeMeshExtendStencil();
	testReMesh();
	testNeuroMeshLinear();
	testNeuroMeshBranching();
	testIntersectVoxel();
	testCubeMeshFillTwoDimSurface();
	testCubeMeshFillThreeDimSurface();
	testCubeMeshJunctionTwoDimSurface();
	testCubeMeshJunctionThreeDimSurface();
	testCubeMeshJunctionDiffSizeMesh();
}
