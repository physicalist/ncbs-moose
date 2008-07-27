/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2008 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <utility/utility.h>

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "../basecode/moose.h"
#include "../element/Neutral.h"
#include "../shell/Shell.h"

extern bool setupProxyMsg(
	unsigned int srcNode, Id proxy, unsigned int srcFuncId,
	Id dest, int destMsg );


using namespace std;

static Element* pj = 0;
static const Finfo* stepFinfo;

/**
 * Initializes MPI as well as scheduling and cross-node shell messaging.
 * Returns node number.
 */
unsigned int initMPI( int argc, char** argv )
{
#ifdef USE_MPI
	MPI::Init( argc, argv );
	unsigned int totalNodes = MPI::COMM_WORLD.Get_size();
	unsigned int myNode = MPI::COMM_WORLD.Get_rank();

	// If not done here, the Shell uses defaults suitable for one node.
	Shell::setNodes( myNode, totalNodes );

	cerr << myNode << ".2a\n";

	bool glug = (argc == 2 && strncmp( argv[1], "-m", 2 ) == 0 );
	while ( glug );

	return myNode;
#else // USE_MPI
	totalNodes = 1;
	return 0;
#endif // USE_MPI
}


/**
 * Initializes parallel scheduling.
 */
void initParSched()
{
#ifdef USE_MPI
	unsigned int totalnodes = Shell::numNodes();
	unsigned int myNode = Shell::myNode();

	// Breakpoint for parallel debugging

	Element* postmasters =
			Neutral::createArray( "PostMaster", "post", 
			Id::shellId(), Id::postId( 0 ), totalnodes );
	assert( postmasters->numEntries() == totalnodes );
	cerr << myNode << ".2b\n";
	for ( unsigned int i = 0; i < totalnodes; i++ ) {
		Eref pe = Eref( postmasters, i );
		set< unsigned int >( pe, "remoteNode", i );
	}
	cerr << myNode << ".2c\n";
	// This one handles parser and postmaster scheduling.
	Id sched( "/sched" );
	Id cj( "/sched/cj" );
	Id t0id( "/sched/cj/t0" );
	if ( t0id.good() ) {
		set( t0id(), "destroy" );
	}
	pj =
		Neutral::create( "ClockJob", "pj", sched, Id::scratchId() );
	Element* t0 =
			Neutral::create( "ParTick", "t0", cj, Id::scratchId() );
	set< int >( t0, "barrier", 1 ); // when running, ensure sync after t0

	Element* pt0 =
			Neutral::create( "ParTick", "t0", pj->id(), Id::scratchId() );
	set< int >( pt0, "barrier", 1 ); // when running, ensure sync after t0

	///////////////////////////////////////////////////////////////////
	//	Here we connect up the postmasters to the shell and the ParTick.
	///////////////////////////////////////////////////////////////////
	Eref shellE = Id::shellId().eref();
	Element* shell = shellE.e;
	const Finfo* parallelFinfo = shell->findFinfo( "parallel" );
	assert( parallelFinfo != 0 );
	const Finfo* pollFinfo = shell->findFinfo( "pollSrc" );
	assert( pollFinfo != 0 );
	const Finfo* tickFinfo = t0->findFinfo( "parTick" );
	assert( tickFinfo != 0 );

	stepFinfo = pj->findFinfo( "step" );
	assert( stepFinfo != 0 );

	SetConn c( shellE );
	// Here we need to set up the local connections that will become
	// connections between shell on this node to all other onodes
	// Shell::addParallelSrc( &c, 

	bool ret;
	for ( unsigned int i = 0; i < totalnodes; i++ ) {
		if ( i != myNode) {
			ret = setupProxyMsg( i, 
				Id::shellId(), parallelFinfo->asyncFuncId(), 
				Id::shellId(), parallelFinfo->msg()
			);
			assert( ret != 0 );
			assert( Id::lastId().isProxy() );
			assert( Id::lastId().eref().data() == 
				Id::postId( i ).eref().data() ) ;
		}
	}

	ret = shellE.add( "pollSrc", pj, "step" );
	assert( ret );

	vector< Element* >::iterator j;
	ret = shellE.add( "pollSrc", pj, "step" );
	// ret = pollFinfo->add( shell, pj, pj->findFinfo( "step" ) );
	assert( ret );
	Eref pe = Id::postId( Id::AnyIndex ).eref();
	ret = Eref( t0 ).add( "parTick", pe , "parTick",
		ConnTainer::One2All );
	assert( ret );
	ret = Eref( pt0 ).add( "parTick", pe, "parTick",
		ConnTainer::One2All );
	assert( ret );

	// cout << "On " << myNode << ", shell: " << shell->name() << endl;
	// shell->dumpMsgInfo();
	set( cj.eref(), "resched" );
	set( pj, "resched" );
	set( cj.eref(), "reinit" );
	set( pj, "reinit" );

	cerr << myNode << ".2d\n";

	MPI::COMM_WORLD.Barrier();
	if ( myNode == 0 )
		cout << "\nInitialized " << totalnodes << " nodes\n";
	MPI::COMM_WORLD.Barrier();
#endif // USE_MPI
}

void terminateMPI( unsigned int myNode )
{
#ifdef USE_MPI
	Eref shell = Id::shellId().eref();
	if ( myNode != 0 ) {
		bool ret = set( shell, "poll" );
		assert( ret );
	}
	MPI::Finalize();
#endif // USE_MPI
}

void pollPostmaster()
{
	if ( pj != 0 ) {
		/*
		if ( Shell::numNodes() > 1 )
			cout << "Polling postmaster on node " << Shell::myNode() << endl;
			*/
		bool ret = set< int >( pj, stepFinfo, 1 );
		assert( ret );
	}
}

#ifndef USE_MPI
void testParMsgOnSingleNode()
{
	// Dummy. The actual routine is in parallel/PostMaster.cpp.
}
void testPostMaster()
{
	// Dummy. The actual routine is in parallel/PostMaster.cpp.
}
#endif
