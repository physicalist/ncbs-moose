/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ReduceBase.h"
#include "ReduceMax.h"
#include "Shell.h"
#ifdef USE_MPI
#include <mpi.h>
#endif
#include "../scheduling/Tick.h"
#include "../scheduling/TickMgr.h"
#include "../scheduling/TickPtr.h"
#include "../scheduling/Clock.h"
#include "../scheduling/testScheduling.h"

#include "../builtins/Arith.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "SingleMsg.h"
#include "Wildcard.h"

void testCreateDelete()
{
	
	Eref ser = Id().eref();
	Id testId = Id::nextId();
	vector< unsigned int > dimensions;
	dimensions.push_back( 1 );
	// Need to get the id back so that I can delete it later.
	bool ret = SetGet5< string, Id, Id, string, vector< unsigned int > >::set( ser, "create", "Neutral", Id(), testId , "testCreate", dimensions );
	assert( ret );

	ret = SetGet1< Id >::set( ser, "delete", testId );
	assert( ret );

	cout << "." << flush;
}


/**
 * Tests Create and Delete calls issued through the parser interface,
 * which internally sets up blocking messaging calls.
 */
void testShellParserCreateDelete()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

	vector< unsigned int > dimensions;
	dimensions.push_back( 1 );
	Id child = shell->doCreate( "Neutral", Id(), "test", dimensions );

	shell->doDelete( child );
	cout << "." << flush;
}

/**
 * Tests traversal through parents and siblings.
 */
void testTreeTraversal()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

	vector< unsigned int > dimensions;
	dimensions.push_back( 1 );
	Id f1 = shell->doCreate( "Neutral", Id(), "f1", dimensions );
	assert( f1 != Id() );
	Id f2a = shell->doCreate( "Neutral", f1, "f2a", dimensions );
	assert( f2a != Id() );
	Id f2b = shell->doCreate( "Neutral", f1, "f2b", dimensions );
	assert( f2b != Id() );
	Id f2c = shell->doCreate( "Neutral", f1, "f2c", dimensions );
	assert( f2c != Id() );
	Id f3aa = shell->doCreate( "Neutral", f2a, "f3aa", dimensions );
	assert( f3aa != Id() );
	Id f3ab = shell->doCreate( "Neutral", f2a, "f3ab", dimensions );
	assert( f3ab != Id() );
	Id f3ba = shell->doCreate( "Neutral", f2b, "f3ba", dimensions );
	assert( f3ba != Id() );

	////////////////////////////////////////////////////////////////
	// Checking for own Ids
	////////////////////////////////////////////////////////////////
	FullId me = Field< FullId >::get( f3aa.eref(), "me" );
	assert( me == FullId( f3aa, 0 ) );
	me = Field< FullId >::get( f3ba.eref(), "me" );
	assert( me == FullId( f3ba, 0 ) );
	me = Field< FullId >::get( f2c.eref(), "me" );
	assert( me == FullId( f2c, 0 ) );

	////////////////////////////////////////////////////////////////
	// Checking for parent Ids
	////////////////////////////////////////////////////////////////
	FullId pa = Field< FullId >::get( f3aa.eref(), "parent" );
	assert( pa == FullId( f2a, 0 ) );
	pa = Field< FullId >::get( f3ab.eref(), "parent" );
	assert( pa == FullId( f2a, 0 ) );
	pa = Field< FullId >::get( f2b.eref(), "parent" );
	assert( pa == FullId( f1, 0 ) );
	pa = Field< FullId >::get( f1.eref(), "parent" );
	assert( pa == FullId( Id(), 0 ) );

	cout << "." << flush;

	////////////////////////////////////////////////////////////////
	// Checking for child Id lists
	////////////////////////////////////////////////////////////////
	vector< Id > kids = Field< vector< Id > >::get( f1.eref(), "children" );
	assert( kids.size() == 3 );
	assert( kids[0] == f2a );
	assert( kids[1] == f2b );
	assert( kids[2] == f2c );

	kids = Field< vector< Id > >::get( f2a.eref(), "children" );
	assert( kids.size() == 2 );
	assert( kids[0] == f3aa );
	assert( kids[1] == f3ab );
	
	kids = Field< vector< Id > >::get( f2b.eref(), "children" );
	assert( kids.size() == 1 );
	assert( kids[0] == f3ba );

	kids = Field< vector< Id > >::get( f2c.eref(), "children" );
	assert( kids.size() == 0 );

	cout << "." << flush;

	////////////////////////////////////////////////////////////////
	// Checking path string generation.
	////////////////////////////////////////////////////////////////
	string path = Field< string >::get( f3aa.eref(), "path" );
	assert( path == "/f1/f2a/f3aa" );
	path = Field< string >::get( f3ab.eref(), "path" );
	assert( path == "/f1/f2a/f3ab" );
	path = Field< string >::get( f3ba.eref(), "path" );
	assert( path == "/f1/f2b/f3ba" );

	path = Field< string >::get( f2a.eref(), "path" );
	assert( path == "/f1/f2a" );
	path = Field< string >::get( f2b.eref(), "path" );
	assert( path == "/f1/f2b" );
	path = Field< string >::get( f2c.eref(), "path" );
	assert( path == "/f1/f2c" );
	path = Field< string >::get( f1.eref(), "path" );
	assert( path == "/f1" );
	path = Field< string >::get( Id().eref(), "path" );
	assert( path == "/" );

	cout << "." << flush;
	////////////////////////////////////////////////////////////////
	// Checking finding Ids from path
	////////////////////////////////////////////////////////////////

	shell->setCwe( Id() );
	assert( shell->doFind( "/f1/f2a/f3aa" ) == f3aa );
	assert( shell->doFind( "/f1/f2a/f3ab" ) == f3ab );
	assert( shell->doFind( "/f1/f2b/f3ba" ) == f3ba );
	assert( shell->doFind( "/f1/f2b/f3ba/" ) == f3ba );
	assert( shell->doFind( "/f1/f2b/f3ba/.." ) == f2b );
	assert( shell->doFind( "f1/f2b/f3ba" ) == f3ba );
	assert( shell->doFind( "./f1/f2b/f3ba" ) == f3ba );
	assert( shell->doFind( "./f1/f2b/../f2a/f3aa" ) == f3aa );

	shell->setCwe( f2b );
	assert( shell->doFind( "." ) == f2b );
	assert( shell->doFind( "f3ba" ) == f3ba );
	assert( shell->doFind( "f3ba/" ) == f3ba );
	assert( shell->doFind( "f3ba/.." ) == f2b );
	assert( shell->doFind( "f3ba/../../f2a/f3aa" ) == f3aa );
	assert( shell->doFind( "../f2a/f3ab" ) == f3ab );
	
	cout << "." << flush;
	////////////////////////////////////////////////////////////////
	// Checking getChild
	////////////////////////////////////////////////////////////////
	// Neutral* f1data = reinterpret_cast< Neutral* >( f1.eref().data() );
	assert( f2a == Neutral::child( f1.eref(), "f2a" ) );
	assert( f2b == Neutral::child( f1.eref(), "f2b" ) );
	assert( f2c == Neutral::child( f1.eref(), "f2c" ) );

	shell->doDelete( f1 );
	cout << "." << flush;
}

/// Test the Neutral::isDescendant
void testDescendant()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

	vector< unsigned int > dimensions;
	dimensions.push_back( 1 );
	Id f1 = shell->doCreate( "Neutral", Id(), "f1", dimensions );
	Id f2a = shell->doCreate( "Neutral", f1, "f2a", dimensions );
	Id f2b = shell->doCreate( "Neutral", f1, "f2b", dimensions );
	Id f3aa = shell->doCreate( "Neutral", f2a, "f3aa", dimensions );

	assert( Neutral::isDescendant( f3aa, Id() ) );
	assert( Neutral::isDescendant( f3aa, f1 ) );
	assert( Neutral::isDescendant( f3aa, f2a ) );
	assert( Neutral::isDescendant( f3aa, f3aa ) );
	assert( !Neutral::isDescendant( f3aa, f2b ) );

	assert( Neutral::isDescendant( f2b, Id() ) );
	assert( Neutral::isDescendant( f2b, f2b ) );
	assert( !Neutral::isDescendant( f2b, f2a ) );
	assert( !Neutral::isDescendant( f2b, f3aa ) );

	assert( Neutral::isDescendant( f1, Id() ) );
	assert( Neutral::isDescendant( f1, f1 ) );
	assert( !Neutral::isDescendant( f1, f2a ) );
	assert( !Neutral::isDescendant( f1, f2b ) );
	assert( !Neutral::isDescendant( f1, f3aa ) );

	shell->doDelete( f1 );
	cout << "." << flush;
}

/// Utility function to check a commonly used tree structure
void verifyKids( Id f1, Id f2a, Id f2b, Id f3, Id f4a, Id f4b )
{
	Neutral* f1Data = reinterpret_cast< Neutral* >( f1.eref().data() );
	vector< Id > kids;
 	f1Data->children( f1.eref(), kids );
	assert( kids.size() == 2 );
	assert( kids[0] == f2a );
	assert( kids[1] == f2b );

	vector< Id > tree;
	Qinfo q;
	// Neutral::buildTree is depth-first.
	int num = f1Data->buildTree( f1.eref(), &q, tree );
	assert( num == 6 );
	assert( tree.size() == 6 );
	assert( tree[0] == f1 );
	assert( tree[1] == f2a );
	assert( tree[2] == f3 );
	assert( tree[3] == f4a );
	assert( tree[4] == f4b );
	assert( tree[5] == f2b );
}

/// Test the Neutral::children and buildTree
void testChildren()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

	vector< unsigned int > dimensions;
	dimensions.push_back( 1 );
	Id f1 = shell->doCreate( "Neutral", Id(), "f1", dimensions );
	Id f2a = shell->doCreate( "Neutral", f1, "f2a", dimensions );
	Id f2b = shell->doCreate( "Neutral", f1, "f2b", dimensions );
	Id f3 = shell->doCreate( "Neutral", f2a, "f3", dimensions );
	Id f4a = shell->doCreate( "Neutral", f3, "f4a", dimensions );
	Id f4b = shell->doCreate( "Neutral", f3, "f4b", dimensions );

	verifyKids( f1, f2a, f2b, f3, f4a, f4b );

	shell->doDelete( f1 );
	cout << "." << flush;
}

void testMove()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

	vector< unsigned int > dimensions;
	dimensions.push_back( 1 );
	/*
	Id f1 = shell->doCreate( "Neutral", Id(), "f1", dimensions );
	Id f2a = shell->doCreate( "Neutral", f1, "f2a", dimensions );
	Id f2b = shell->doCreate( "Neutral", f1, "f2b", dimensions );
	Id f3aa = shell->doCreate( "Neutral", f2a, "f3aa", dimensions );
	*/

	Id f1 = shell->doCreate( "Neutral", Id(), "f1", dimensions );
	Id f2a = shell->doCreate( "Neutral", f1, "f2a", dimensions );
	Id f2b = shell->doCreate( "Neutral", f1, "f2b", dimensions );
	Id f3 = shell->doCreate( "Neutral", f2a, "f3", dimensions );
	Id f4a = shell->doCreate( "Neutral", f3, "f4a", dimensions );
	Id f4b = shell->doCreate( "Neutral", f3, "f4b", dimensions );
	verifyKids( f1, f2a, f2b, f3, f4a, f4b );

	FullId pa = Field< FullId >::get( f4a.eref(), "parent" );
	assert( pa == FullId( f3, 0 ) );
	pa = Field< FullId >::get( f2a.eref(), "parent" );
	assert( pa == FullId( f1, 0 ) );
	string path = Field< string >::get( f4a.eref(), "path" );
	assert( path == "/f1/f2a/f3/f4a" );
	Neutral* f1data = reinterpret_cast< Neutral* >( f1.eref().data() );

	vector< Id > kids = f1data->getChildren( f1.eref(), 0 );
	assert( kids.size() == 2 );
	assert( kids[0] == f2a );
	assert( kids[1] == f2b );

	Neutral* f3data = reinterpret_cast< Neutral* >( f3.eref().data() );
	kids = f3data->getChildren( f3.eref(), 0 );
	assert( kids.size() == 2 );
	assert( kids[0] == f4a );
	assert( kids[1] == f4b );

	//////////////////////////////////////////////////////////////////
	shell->doMove( f4a, f1 );
	//////////////////////////////////////////////////////////////////

	pa = Field< FullId >::get( f4a.eref(), "parent" );
	assert( pa == FullId( f1, 0 ) );

	kids = f1data->getChildren( f1.eref(), 0 );
	assert( kids.size() == 3 );
	assert( kids[0] == f2a );
	assert( kids[1] == f2b );
	assert( kids[2] == f4a );
	kids = f3data->getChildren( f3.eref(), 0 );
	assert( kids.size() == 1 );
	assert( kids[0] == f4b );

	//////////////////////////////////////////////////////////////////
	shell->doMove( f2a, f4a );
	//////////////////////////////////////////////////////////////////
	pa = Field< FullId >::get( f2a.eref(), "parent" );
	assert( pa == FullId( f4a, 0 ) );
	path = Field< string >::get( f4b.eref(), "path" );
	assert( path == "/f1/f4a/f2a/f3/f4b" );

	kids = f1data->getChildren( f1.eref(), 0 );
	assert( kids[0] == f2b );
	assert( kids[1] == f4a );

	shell->doDelete( f1 );
	cout << "." << flush;
}

void testCopy()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

	vector< unsigned int > dimensions;
	dimensions.push_back( 1 );
	Id f1 = shell->doCreate( "Neutral", Id(), "f1", dimensions );
	Id f2a = shell->doCreate( "Neutral", f1, "f2a", dimensions );
	Id f2b = shell->doCreate( "Neutral", f1, "f2b", dimensions );
	Id f3 = shell->doCreate( "Neutral", f2a, "f3", dimensions );
	Id f4a = shell->doCreate( "Neutral", f3, "f4a", dimensions );
	Id f4b = shell->doCreate( "Neutral", f3, "f4b", dimensions );

	verifyKids( f1, f2a, f2b, f3, f4a, f4b );

	FullId pa = Field< FullId >::get( f3.eref(), "parent" );
	assert( pa == FullId( f2a, 0 ) );
	pa = Field< FullId >::get( f2a.eref(), "parent" );
	assert( pa == FullId( f1, 0 ) );
	string path = Field< string >::get( f3.eref(), "path" );
	assert( path == "/f1/f2a/f3" );

	//////////////////////////////////////////////////////////////////
	Id dupf2a = shell->doCopy( f2a, Id(), "TheElephantsAreLoose", 1, 0 );
	//////////////////////////////////////////////////////////////////

	verifyKids( f1, f2a, f2b, f3, f4a, f4b );

	assert( dupf2a != Id() );
	// pa = Field< FullId >::get( dupf2a.eref(), "parent" );
	// assert( pa == FullId( f1, 0 ) );
	assert( dupf2a()->getName() == "TheElephantsAreLoose" );
	Neutral* f2aDupData = reinterpret_cast< Neutral* >( dupf2a.eref().data() );
	Id dupf3 = f2aDupData->child( dupf2a.eref(), "f3" );
	assert( dupf3 != Id() );
	assert( dupf3 != f3 );
	assert( dupf3()->getName() == "f3" );
	vector< Id > kids = f2aDupData->getChildren( dupf2a.eref(), 0 );
	assert( kids.size() == 1 );
	assert( kids[0] == dupf3 );

	Neutral* f3DupData = reinterpret_cast< Neutral* >( dupf3.eref().data());
	assert( f3DupData->getParent( dupf3.eref(), 0 ) == FullId( dupf2a, 0 ));
	kids = f3DupData->getChildren( dupf3.eref(), 0 );
	assert( kids.size() == 2 );
	assert( kids[0]()->getName() == "f4a" );
	assert( kids[1]()->getName() == "f4b" );

	shell->doDelete( f1 );
	shell->doDelete( dupf2a );
	cout << "." << flush;
}


// Here we create the element independently on each node, and connect
// it up independently. Using the doAddMsg we will be able to do this
// automatically on all nodes.
void testShellParserStart()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

	shell->doSetClock( 0, 5.0 );
	shell->doSetClock( 1, 2.0 );
	shell->doSetClock( 2, 2.0 );
	shell->doSetClock( 3, 1.0 );
	shell->doSetClock( 4, 3.0 );
	shell->doSetClock( 5, 5.0 );


	const Cinfo* testSchedCinfo = TestSched::initCinfo();
	vector< unsigned int > dims;
	Id tsid = Id::nextId();
	Element* tse = new Element( tsid, testSchedCinfo, "tse", dims );

	// testThreadSchedElement tse;
	Eref ts( tse, 0 );
	Element* ticke = Id( 2 )();
	Eref er0( ticke, DataId( 0, 0 ) );
	Eref er1( ticke, DataId( 0, 1 ) );
	Eref er2( ticke, DataId( 0, 2 ) );
	Eref er3( ticke, DataId( 0, 3 ) );
	Eref er4( ticke, DataId( 0, 4 ) );
	Eref er5( ticke, DataId( 0, 5 ) );

	// No idea what FuncId to use here. Assume 0.
	FuncId f( 0 );
	SingleMsg m0( Msg::nextMsgId(), er0, ts ); 
	er0.element()->addMsgAndFunc( m0.mid(), f, 0 );
	SingleMsg m1( Msg::nextMsgId(), er1, ts ); 
	er1.element()->addMsgAndFunc( m1.mid(), f, 1 );
	SingleMsg m2( Msg::nextMsgId(), er2, ts ); 
	er2.element()->addMsgAndFunc( m2.mid(), f, 2 );
	SingleMsg m3( Msg::nextMsgId(), er3, ts ); 
	er3.element()->addMsgAndFunc( m3.mid(), f, 3 );
	SingleMsg m4( Msg::nextMsgId(), er4, ts ); 
	er4.element()->addMsgAndFunc( m4.mid(), f, 4 );
	SingleMsg m5( Msg::nextMsgId(), er5, ts ); 
	er5.element()->addMsgAndFunc( m5.mid(), f, 5 );

	if ( shell->myNode() != 0 )
		return;

	shell->doStart( 10 );

	tsid.destroy();
	cout << "." << flush;
}

/**
 * Tests Shell operations carried out on multiple nodes
 */
void testInterNodeOps() // redundant.
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	Id child;
	if ( shell->myNode() == 0 ) {
		vector< unsigned int > dimensions;
		dimensions.push_back( 6139 );
		child = shell->doCreate( "Neutral", Id(), "test", dimensions );
	} 
	// cout << shell->myNode() << ": testInterNodeOps: #entries = " << child()->dataHandler()->numData() << endl;

	shell->doDelete( child );
	// child.destroy();
	cout << "." << flush;
}

void testShellSetGet()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	const unsigned int size = 100;
	vector< unsigned int > dimensions;
	dimensions.push_back( size );
	vector< double > val;

	// Set up the objects.
	Id a1 = shell->doCreate( "Arith", Id(), "a1", dimensions );

	// cout << Shell::myNode() << ": testShellSetGet: data here = (" << a1()->dataHandler()->begin() << " to " << a1()->dataHandler()->end() << ")" << endl;
	for ( unsigned int i = 0; i < size; ++i ) {
		val.push_back( i * i * i ); // use i^3 as a simple test.
		bool ret = SetGet1< double >::set( Eref( a1(), i ), "set_outputValue", i * i );
		assert( ret );
	}
	for ( unsigned int i = 0; i < size; ++i ) {
		double x = Field< double >::get( Eref( a1(), i ), "outputValue" );
		assert( doubleEq( x, i * i ) );
	}
	bool ret = SetGet1< double >::setVec( Eref( a1(), 0 ), "set_outputValue", val );
	assert( ret );
	for ( unsigned int i = 0; i < size; ++i ) {
		double x = Field< double >::get( Eref( a1(), i ), "outputValue" );
		// cout << i << "	x=" << x << "	i^3=" << i * i * i << endl;
		assert( doubleEq( x, i * i * i ) );
	}

	shell->doDelete( a1 );
	cout << "." << flush;
}

void testShellSetGetVec()
{
	/*
	unsigned int numCells = 65535;
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	vector< unsigned int > dimensions;
	dimensions.push_back( numCells );
	Id child = shell->doCreate( "IntFire", Id(), "test", dimensions );
	for ( unsigned int i = 0; i < numCells; ++i )
		numSyn.push_back( i % 32 );
	shell->doSetUintVec( child, DataId( i ), "numSynapses", numSyn );
	numSyn.clear();
	shell->doGetUintVec( child, DataId( i ), "numSynapses", numSyn );
	for ( unsigned int i = 0; i < numCells; ++i )
		assert( numSyn[i] == i % 32 );
	// Also want to assign each of the synapses to a different value.
	
	shell->doDelete( child );
	cout << "." << flush;
	*/
}

bool setupSched( Shell* shell, FullId& tick, Id dest )
{
	MsgId ret = shell->doAddMsg( "OneToAll", 
		tick, "proc0", FullId( dest, 0 ), "proc" );
	return ( ret != Msg::badMsg );
}

bool checkArg1( Id e, 
	double v0, double v1, double v2, double v3, double v4 )
{
	bool ret = 1;
	bool report = 0;
	Eref e0( e(), 0 );
	double val = reinterpret_cast< Arith* >( e0.data() )->getArg1();
	ret = ret && ( fabs( val - v0 ) < 1e-6 );
	if (report) cout << "( " << v0 << ", " << val << " ) ";

	Eref e1( e(), 1 );
	val = reinterpret_cast< Arith* >( e1.data() )->getArg1();
	ret = ret && ( fabs( val - v1 ) < 1e-6 );
	if (report) cout << "( " << v1 << ", " << val << " ) ";

	Eref e2( e(), 2 );
	val = reinterpret_cast< Arith* >( e2.data() )->getArg1();
	ret = ret && ( fabs( val - v2 ) < 1e-6 );
	if (report) cout << "( " << v2 << ", " << val << " ) ";

	Eref e3( e(), 3 );
	val = reinterpret_cast< Arith* >( e3.data() )->getArg1();
	ret = ret && ( fabs( val - v3 ) < 1e-6 );
	if (report) cout << "( " << v3 << ", " << val << " ) ";

	Eref e4( e(), 4 );
	val = reinterpret_cast< Arith* >( e4.data() )->getArg1();
	ret = ret && ( fabs( val - v4 ) < 1e-6 );
	if (report) cout << "( " << v4 << ", " << val << " )\n";

	return ret;
}

bool checkOutput( Id e, 
	double v0, double v1, double v2, double v3, double v4 )
{
	bool ret = 1;
	bool report = 0;

	vector< double > correct;
	correct.push_back( v0 );
	correct.push_back( v1 );
	correct.push_back( v2 );
	correct.push_back( v3 );
	correct.push_back( v4 );
	vector< double > retVec;
	Field< double >::getVec( Eref( e(), DataId::any() ), "outputValue", retVec );
	assert( retVec.size() == 5 );

	for ( unsigned int i = 0; i < 5; ++i ) {
		ret = ret & doubleEq( retVec[i], correct[i] );
		if (report) cout << "( " << correct[i] << ", " << retVec[i] << " ) ";
	}
	return ret;
}

void testShellAddMsg()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	vector< unsigned int > dimensions;
	dimensions.push_back( 5 );


	///////////////////////////////////////////////////////////
	// Set up the objects.
	///////////////////////////////////////////////////////////
	Id a1 = shell->doCreate( "Arith", Id(), "a1", dimensions );
	Id a2 = shell->doCreate( "Arith", Id(), "a2", dimensions );

	Id b1 = shell->doCreate( "Arith", Id(), "b1", dimensions );
	Id b2 = shell->doCreate( "Arith", Id(), "b2", dimensions );

	Id c1 = shell->doCreate( "Arith", Id(), "c1", dimensions );
	Id c2 = shell->doCreate( "Arith", Id(), "c2", dimensions );

	Id d1 = shell->doCreate( "Arith", Id(), "d1", dimensions );
	Id d2 = shell->doCreate( "Arith", Id(), "d2", dimensions );

	Id e1 = shell->doCreate( "Arith", Id(), "e1", dimensions );
	Id e2 = shell->doCreate( "Arith", Id(), "e2", dimensions );

	Id f1 = shell->doCreate( "Arith", Id(), "f1", dimensions );
	Id f2 = shell->doCreate( "Arith", Id(), "f2", dimensions );

	Id g1 = shell->doCreate( "Arith", Id(), "g1", dimensions );
	Id g2 = shell->doCreate( "Arith", Id(), "g2", dimensions );


	///////////////////////////////////////////////////////////
	// Set up messaging
	///////////////////////////////////////////////////////////
	// Should give 04000
	MsgId m1 = shell->doAddMsg( "Single", 
		FullId( a1, 3 ), "output", FullId( a2, 1 ), "arg3" );
	assert( m1 != Msg::badMsg );

	// Should give 33333
	MsgId m2 = shell->doAddMsg( "OneToAll", 
		FullId( b1, 2 ), "output", FullId( b2, 0 ), "arg3" );
	assert( m2 != Msg::badMsg );

	// Should give 12345
	MsgId m3 = shell->doAddMsg( "OneToOne", 
		FullId( c1, 0 ), "output", FullId( c2, 0 ), "arg3" );
	assert( m3 != Msg::badMsg );

	// Should give 01234
	MsgId m4 = shell->doAddMsg( "Diagonal", 
		FullId( d1, 0 ), "output", FullId( d2, 0 ), "arg3" );
	assert( m4 != Msg::badMsg );

	// Should give 54321
	MsgId m5 = shell->doAddMsg( "Sparse", 
		FullId( e1, 0 ), "output", FullId( e2, 0 ), "arg3" );
	assert( m5 != Msg::badMsg );

	const Msg* m5p = Msg::getMsg( m5 );
	Eref m5er = m5p->manager();

	bool ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5er, "setEntry", 0, 4, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5er, "setEntry", 1, 3, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5er, "setEntry", 2, 2, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5er, "setEntry", 3, 1, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5er, "setEntry", 4, 0, 0 );
	assert( ret );

	// Should give 15 15 15 15 15
	for ( unsigned int i = 0; i < 5; ++i ) {
		MsgId m6 = shell->doAddMsg( "OneToAll", 
			FullId( f1, i ), "output", FullId( f2, i ), "arg3" );
		assert( m6 != Msg::badMsg );
	}

	// Should give 14 13 12 11 10
	MsgId m7 = shell->doAddMsg( "Sparse", 
		FullId( g1, 0 ), "output", FullId( g2, 0 ), "arg3" );
	assert( m7 != Msg::badMsg );
	const Msg* m7p = Msg::getMsg( m7 );
	Eref m7er = m7p->manager();
	for ( unsigned int i = 0; i < 5; ++i ) {
		for ( unsigned int j = 0; j < 5; ++j ) {
			if ( i != j ) {
				ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
				m7er, "setEntry", i, j, 0 );
				assert( ret );
			}
		}
	}

	///////////////////////////////////////////////////////////
	// Set up scheduling
	///////////////////////////////////////////////////////////
	shell->doSetClock( 0, 1.0 );

	FullId tick( Id( 2 ), 0 );
	// I want to compare the # of process msgs before and after.
	vector< Id > tgts;
	const SrcFinfo* sf = dynamic_cast< const SrcFinfo* >(
		Tick::initCinfo()->findFinfo( "process0" ) );
	assert( sf );
	unsigned int numTgts = tick.eref().element()->getOutputs( tgts, 
		sf );
	assert( numTgts == 0 );
	ret = setupSched( shell, tick, a1 ); assert( ret );
	ret = setupSched( shell, tick, a2 ); assert( ret );
	ret = setupSched( shell, tick, b1 ); assert( ret );
	ret = setupSched( shell, tick, b2 ); assert( ret );
	ret = setupSched( shell, tick, c1 ); assert( ret );
	ret = setupSched( shell, tick, c2 ); assert( ret );
	ret = setupSched( shell, tick, d1 ); assert( ret );
	ret = setupSched( shell, tick, d2 ); assert( ret );
	ret = setupSched( shell, tick, e1 ); assert( ret );
	ret = setupSched( shell, tick, e2 ); assert( ret );
	ret = setupSched( shell, tick, f1 ); assert( ret );
	ret = setupSched( shell, tick, f2 ); assert( ret );
	ret = setupSched( shell, tick, g1 ); assert( ret );
	ret = setupSched( shell, tick, g2 ); assert( ret );

	numTgts = tick.eref().element()->getOutputs( tgts, sf );
	assert( numTgts == 14 );

	///////////////////////////////////////////////////////////
	// Set up initial conditions
	///////////////////////////////////////////////////////////
	
	shell->doReinit();

	vector< double > init; // 12345
	for ( unsigned int i = 1; i < 6; ++i )
		init.push_back( i );
	ret = SetGet1< double >::setVec( a1.eref(), "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( b1.eref(), "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( c1.eref(), "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( d1.eref(), "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( e1.eref(), "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( f1.eref(), "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( g1.eref(), "arg1", init ); // 12345
	assert( ret );

	assert( doubleEq( Field< double >::get( Eref( a1(), 0 ), "arg1Value" ), 1 ) );
	assert( doubleEq( Field< double >::get( Eref( a1(), 1 ), "arg1Value" ), 2 ) );
	assert( doubleEq( Field< double >::get( Eref( a1(), 2 ), "arg1Value" ), 3 ) );
	assert( doubleEq( Field< double >::get( Eref( a1(), 3 ), "arg1Value" ), 4 ) );
	assert( doubleEq( Field< double >::get( Eref( a1(), 4 ), "arg1Value" ), 5 ) );

	vector< double > retVec( 0 );
	Field< double >::getVec( Eref( a1(), DataId::any() ), "arg1Value", retVec );
	for ( unsigned int i = 0; i < 5; ++i )
		assert( doubleEq( retVec[i], i + 1 ) );

	retVec.resize( 0 );
	Field< double >::getVec( Eref( b1(), DataId::any() ), "arg1Value", retVec );
	for ( unsigned int i = 0; i < 5; ++i )
		assert( doubleEq( retVec[i], i + 1 ) );

	retVec.resize( 0 );
	Field< double >::getVec( Eref( c1(), DataId::any() ), "arg1Value", retVec );
	for ( unsigned int i = 0; i < 5; ++i )
		assert( doubleEq( retVec[i], i + 1 ) );


	///////////////////////////////////////////////////////////
	// Run it
	///////////////////////////////////////////////////////////

	shell->doStart( 2 );

	// Clock* clock = reinterpret_cast< Clock* >( Id(1).eref().data() );
	// clock->printCounts();
	// Qinfo::reportQ();

	///////////////////////////////////////////////////////////
	// Check output.
	///////////////////////////////////////////////////////////
	
	ret = checkOutput( a1, 1, 2, 3, 4, 5 );
	assert( ret );
	ret = checkOutput( b1, 1, 2, 3, 4, 5 );
	assert( ret );
	ret = checkOutput( c1, 1, 2, 3, 4, 5 );
	assert( ret );
	ret = checkOutput( d1, 1, 2, 3, 4, 5 );
	assert( ret );
	ret = checkOutput( e1, 1, 2, 3, 4, 5 );
	assert( ret );
	ret = checkOutput( a2, 0, 4, 0, 0, 0 );
	assert( ret );
	ret = checkOutput( b2, 3, 3, 3, 3, 3 );
	assert( ret );
	ret = checkOutput( c2, 1, 2, 3, 4, 5 );
	assert( ret );
	ret = checkOutput( d2, 0, 1, 2, 3, 4 );
	assert( ret );
	ret = checkOutput( e2, 5, 4, 3, 2, 1 );
	assert( ret );
	ret = checkOutput( f2, 15, 15, 15, 15, 15 );
	assert( ret );
	ret = checkOutput( g2, 14, 13, 12, 11, 10 );
	assert( ret );

	///////////////////////////////////////////////////////////
	// Clean up.
	///////////////////////////////////////////////////////////
	shell->doDelete( a1 );
	shell->doDelete( a2 );
	shell->doDelete( b1 );
	shell->doDelete( b2 );
	shell->doDelete( c1 );
	shell->doDelete( c2 );
	shell->doDelete( d1 );
	shell->doDelete( d2 );
	shell->doDelete( e1 );
	shell->doDelete( e2 );
	shell->doDelete( f1 );
	shell->doDelete( f2 );
	shell->doDelete( g1 );
	shell->doDelete( g2 );

	cout << "." << flush;
}

// Very similar to above, except that the tests are done on a copy.
void testCopyMsgOps()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	vector< unsigned int > dimensions;
	Id pa = shell->doCreate( "Neutral", Id(), "pa", dimensions );
	dimensions.push_back( 5 );


	///////////////////////////////////////////////////////////
	// Set up the objects.
	///////////////////////////////////////////////////////////
	Id a1 = shell->doCreate( "Arith", pa, "a1", dimensions );
	Id a2 = shell->doCreate( "Arith", pa, "a2", dimensions );

	Id b1 = shell->doCreate( "Arith", pa, "b1", dimensions );
	Id b2 = shell->doCreate( "Arith", pa, "b2", dimensions );

	Id c1 = shell->doCreate( "Arith", pa, "c1", dimensions );
	Id c2 = shell->doCreate( "Arith", pa, "c2", dimensions );

	Id d1 = shell->doCreate( "Arith", pa, "d1", dimensions );
	Id d2 = shell->doCreate( "Arith", pa, "d2", dimensions );

	Id e1 = shell->doCreate( "Arith", pa, "e1", dimensions );
	Id e2 = shell->doCreate( "Arith", pa, "e2", dimensions );

	///////////////////////////////////////////////////////////
	// Set up initial conditions and some scheduling.
	///////////////////////////////////////////////////////////
	shell->doSetClock( 0, 1.0 );
	shell->doReinit();
	bool ret = 0;
	vector< double > init; // 12345
	for ( unsigned int i = 1; i < 6; ++i )
		init.push_back( i );
	ret = SetGet1< double >::setVec( a1.eref(), "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( b1.eref(), "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( c1.eref(), "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( d1.eref(), "arg1", init ); // 12345
	assert( ret );
	ret = SetGet1< double >::setVec( e1.eref(), "arg1", init ); // 12345
	assert( ret );


	///////////////////////////////////////////////////////////
	// Set up messaging
	///////////////////////////////////////////////////////////
	// Should give 04000
	MsgId m1 = shell->doAddMsg( "Single", 
		FullId( a1, 3 ), "output", FullId( a2, 1 ), "arg1" );
	assert( m1 != Msg::badMsg );

	// Should give 33333
	MsgId m2 = shell->doAddMsg( "OneToAll", 
		FullId( b1, 2 ), "output", FullId( b2, 0 ), "arg1" );
	assert( m2 != Msg::badMsg );

	// Should give 12345
	MsgId m3 = shell->doAddMsg( "OneToOne", 
		FullId( c1, 0 ), "output", FullId( c2, 0 ), "arg1" );
	assert( m3 != Msg::badMsg );

	// Should give 01234
	MsgId m4 = shell->doAddMsg( "Diagonal", 
		FullId( d1, 0 ), "output", FullId( d2, 0 ), "arg1" );
	assert( m4 != Msg::badMsg );

	// Should give 54321
	MsgId m5 = shell->doAddMsg( "Sparse", 
		FullId( e1, 0 ), "output", FullId( e2, 0 ), "arg1" );
	assert( m5 != Msg::badMsg );

	const Msg* m5p = Msg::getMsg( m5 );
	Eref m5er = m5p->manager();

	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5er, "setEntry", 0, 4, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5er, "setEntry", 1, 3, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5er, "setEntry", 2, 2, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5er, "setEntry", 3, 1, 0 );
	assert( ret );
	ret = SetGet3< unsigned int, unsigned int, unsigned int >::set(
		m5er, "setEntry", 4, 0, 0 );
	assert( ret );

	// ret = SetGet1< unsigned int >::set( m5er, "loadBalance", Shell::numCores() );
	assert( ret );

	FullId tick( Id( 2 ), 0 );

	///////////////////////////////////////////////////////////
	// Copy it
	///////////////////////////////////////////////////////////

	Id pa2 = shell->doCopy( pa, Id(), "pa2", 1, 0 );

	///////////////////////////////////////////////////////////
	// Pull out the child Ids.
	///////////////////////////////////////////////////////////
	vector< Id > kids = Field< vector< Id > >::get( pa2.eref(), "children");
	assert ( kids.size() == 10 );
	for ( unsigned int i = 0; i < kids.size(); ++i ) {
		ret = setupSched( shell, tick, kids[i] ); 
		assert( ret );
	}

	unsigned int j = 0;
	assert( kids[j]()->getName() == "a1" ); ++j;
	assert( kids[j]()->getName() == "a2" ); ++j;
	assert( kids[j]()->getName() == "b1" ); ++j;
	assert( kids[j]()->getName() == "b2" ); ++j;
	assert( kids[j]()->getName() == "c1" ); ++j;
	assert( kids[j]()->getName() == "c2" ); ++j;
	assert( kids[j]()->getName() == "d1" ); ++j;
	assert( kids[j]()->getName() == "d2" ); ++j;
	assert( kids[j]()->getName() == "e1" ); ++j;
	assert( kids[j]()->getName() == "e2" ); ++j;

	vector< double > retVec;
	Field< double >::getVec( Eref( kids[0](), DataId::any() ), "arg1Value", retVec );
	assert( retVec.size() == 5 );
	for (unsigned int i = 0; i < 5; ++i )
		assert( doubleEq( retVec[i], init[i] ) );

	///////////////////////////////////////////////////////////
	// Run it
	///////////////////////////////////////////////////////////

	shell->doStart( 2 );

	///////////////////////////////////////////////////////////
	// Check output.
	///////////////////////////////////////////////////////////
	
	ret = checkOutput( kids[1], 0, 4, 0, 0, 0 );
	assert( ret );
	ret = checkOutput( kids[2], 1, 2, 3, 4, 5 );
	assert( ret );
	ret = checkOutput( kids[3], 3, 3, 3, 3, 3 );
	assert( ret );
	ret = checkOutput( kids[5], 1, 2, 3, 4, 5 );
	assert( ret );
	ret = checkOutput( kids[7], 0, 1, 2, 3, 4 );
	assert( ret );
	ret = checkOutput( kids[9], 5, 4, 3, 2, 1 );
	assert( ret );

	///////////////////////////////////////////////////////////
	// Clean up.
	///////////////////////////////////////////////////////////
	shell->doDelete( pa );
	shell->doDelete( pa2 );

	cout << "." << flush;
}

void testShellParserQuit()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	shell->doQuit( );
	cout << "." << flush;
}

void testChopPath()
{
	vector< string > args;

	assert( Shell::chopPath( ".", args ) == 0 );
	assert( args.size() == 1 );
	assert( args[0] == "." );

	assert( Shell::chopPath( "/", args ) == 1 );
	assert( args.size() == 0 );

	assert( Shell::chopPath( "..", args ) == 0 );
	assert( args.size() == 1 );
	assert( args[0] == ".." );

	assert( Shell::chopPath( "./", args ) == 0 );
	assert( args.size() == 1 );
	assert( args[0] == "." );

	assert( Shell::chopPath( "./foo", args ) == 0 );
	assert( args.size() == 2 );
	assert( args[0] == "." );
	assert( args[1] == "foo" );

	assert( Shell::chopPath( "/foo", args ) == 1 );
	assert( args.size() == 1 );
	assert( args[0] == "foo" );

	assert( Shell::chopPath( "foo", args ) == 0 );
	assert( args.size() == 1 );
	assert( args[0] == "foo" );

	assert( Shell::chopPath( "foo/", args ) == 0 );
	assert( args.size() == 1 );
	assert( args[0] == "foo" );

	assert( Shell::chopPath( "/foo/", args ) == 1 );
	assert( args.size() == 1 );
	assert( args[0] == "foo" );

	assert( Shell::chopPath( "foo/bar/zod", args ) == 0 );
	assert( args.size() == 3 );
	assert( args[0] == "foo" );
	assert( args[1] == "bar" );
	assert( args[2] == "zod" );

	cout << "." << flush;
}

void testFindModelParent()
{
	bool findModelParent( Id cwe, const string& path,
		Id& parentId, string& modelName );

	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );

	///////////////////////////////////////////////////////////
	// Set up the objects.
	///////////////////////////////////////////////////////////
	vector< unsigned int > dimensions( 1, 1 );
	Id foo = shell->doCreate( "Neutral", Id(), "foo", dimensions );
	Id zod = shell->doCreate( "Neutral", Id(), "zod", dimensions );
	Id foo2 = shell->doCreate( "Neutral", zod, "foo", dimensions );

	// shell->setCwe( zod );

	string modelName;
	Id parentId;

	//////////////// do not spec model name
	bool ok = findModelParent( zod, "", parentId, modelName );
	assert( ok );
	assert( parentId == zod );
	assert( modelName == "model" );
	modelName = "";

	ok = findModelParent( zod, "/", parentId, modelName );
	assert( ok );
	assert( parentId == Id() );
	assert( modelName == "model" );
	modelName = "";

	ok = findModelParent( zod, "/foo", parentId, modelName );
	assert( ok );
	assert( parentId == foo );
	assert( modelName == "model" );
	modelName = "";

	ok = findModelParent( zod, "foo", parentId, modelName );
	assert( ok );
	assert( parentId == foo2 );
	assert( modelName == "model" );
	modelName = "";

	//////////////// spec model name too
	ok = findModelParent( zod, "bar", parentId, modelName );
	assert( ok );
	assert( parentId == zod );
	assert( modelName == "bar" );
	modelName = "";

	ok = findModelParent( zod, "/bar", parentId, modelName );
	assert( ok );
	assert( parentId == Id() );
	assert( modelName == "bar" );
	modelName = "";

	ok = findModelParent( foo, "/foo/bar", parentId, modelName );
	assert( ok );
	assert( parentId == foo );
	assert( modelName == "bar" );
	modelName = "";

	ok = findModelParent( zod, "foo/bar", parentId, modelName );
	assert( ok );
	assert( parentId == foo2 );
	assert( modelName == "bar" );
	
	shell->doDelete( foo );
	shell->doDelete( foo2 );
	shell->doDelete( zod );
	cout << "." << flush;
}

void testSyncSynapseSize()
{
	Eref sheller = Id().eref();
	Shell* shell = reinterpret_cast< Shell* >( sheller.data() );
	const Finfo* f = Cinfo::find( "IntFire" )->findFinfo( "get_numSynapses" );
	const DestFinfo* df = dynamic_cast< const DestFinfo* >( f );
	assert( df );
	unsigned int size = 1000;
	vector< unsigned int > dims( 1, size );
	Id neuronId = shell->doCreate( "IntFire", Id(), "neurons", dims );
	Id synId( neuronId.value() + 1 );
	Element* syn = synId();

	// Element should exist even if data doesn't
	assert ( syn != 0 );
	assert ( syn->getName() == "synapse" ); 

	assert( syn->dataHandler()->data( 0 ) == 0 );

	assert( syn->dataHandler()->totalEntries() == size );
	assert( syn->dataHandler()->localEntries() == 0 );
	vector< unsigned int > ns( size, 0 );
	for ( unsigned int i = 0; i < size; ++i )
		ns[i] = i;
	bool ret = Field< unsigned int >::setVec( 
		neuronId.eref(), "numSynapses", ns );
	assert( ret );
	// Here we check local entries
	/*
	DataHandler::iterator begin = syn->dataHandler()->begin();
	DataHandler::iterator end = syn->dataHandler()->end();
	assert( syn->dataHandler()->localEntries() == ( size * (size - 1) ) / 2 );
	*/

	shell->doSyncDataHandler( neuronId, df->getFid(), synId );

	assert( syn->dataHandler()->totalEntries() == size * (size - 1 ) );
	// cout << "NumSyn = " << syn.totalEntries() << endl;
	shell->doDelete( neuronId );
	cout << "." << flush;
}

void testShell( )
{
	testChopPath();
	testTreeTraversal();
	testChildren();
//	testCreateDelete();
	// testShellParserQuit();
}

extern void testWildcard();

void testMpiShell( )
{
	testShellParserCreateDelete();
	testTreeTraversal();
	testChildren();
	testDescendant();
	testMove();
	testCopy();
	testShellSetGet();
	testInterNodeOps();
	testShellAddMsg();
	testCopyMsgOps();
	testWildcard();
	// testSyncSynapseSize(); // 23 Feb 2011: commented out to check
	// if this causes the queue problems.

	// Stuff for doLoadModel
	testFindModelParent();
}
