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

extern void testPostMaster();
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
	unsigned int totalnodes = MPI::COMM_WORLD.Get_size();
	unsigned int myNode = MPI::COMM_WORLD.Get_rank();
	bool ret;

	Element* postmasters =
			Neutral::createArray( "PostMaster", "post", 
			Id::shellId(), Id::postId( 0 ), totalnodes );
	for ( unsigned int i = 0; i < totalnodes; i++ ) {
		Eref pe = Eref( postmasters, i );
		set< unsigned int >( pe, "remoteNode", i );
	}
	Id::setNodes( myNode, totalnodes );

	// Breakpoint for parallel debugging
	bool glug = (argc == 2 && strncmp( argv[1], "-m", 2 ) == 0 );
	while ( glug );

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

	///////////////////////////////////////////////////////////////////
	//	Here we connect up the postmasters to the shell and the ParTick.
	///////////////////////////////////////////////////////////////////
	Id shellId( "/shell" );
	Element* shell = shellId();
	const Finfo* parallelFinfo = shell->findFinfo( "parallel" );
	assert( parallelFinfo != 0 );
	const Finfo* pollFinfo = shell->findFinfo( "pollSrc" );
	assert( pollFinfo != 0 );
	const Finfo* tickFinfo = t0->findFinfo( "parTick" );
	assert( tickFinfo != 0 );

	stepFinfo = pj->findFinfo( "step" );
	assert( stepFinfo != 0 );


	Eref shellE = shellId.eref();

	SetConn c( shellE );
	// Here we need to set up the local connections that will become
	// connections between shell on this node to all other onodes
	// Shell::addParallelSrc( &c, 

	for ( unsigned int i = 0; i < totalnodes; i++ ) {
		if ( i != myNode);
		bool ret = setupProxyMsg( i, 
			shellId, parallelFinfo->asyncFuncId(), 
			shellId, parallelFinfo->msg()
		);
		assert( ret != 0 );
	}

	ret = shellE.add( "pollSrc", pj, "step" );
	assert( ret );

	vector< Element* >::iterator j;
	ret = shellE.add( "pollSrc", pj, "step" );
	// ret = pollFinfo->add( shell, pj, pj->findFinfo( "step" ) );
	assert( ret );
	ret = Eref( t0 ).add( "parTick", postmasters , "parTick",
		ConnTainer::One2All );
	assert( ret );
	ret = Eref( pt0 ).add( "parTick", postmasters, "parTick",
		ConnTainer::One2All );
	assert( ret );

	// cout << "On " << myNode << ", shell: " << shell->name() << endl;
	// shell->dumpMsgInfo();
	set( cj.eref(), "resched" );
	set( pj, "resched" );
	set( cj.eref(), "reinit" );
	set( pj, "reinit" );
#ifdef DO_UNIT_TESTS
	MPI::COMM_WORLD.Barrier();
	if ( myNode == 0 )
		cout << "\nInitialized " << totalnodes << " nodes\n";
	MPI::COMM_WORLD.Barrier();
	testPostMaster();
#endif // DO_UNIT_TESTS
	return myNode;
#else // USE_MPI
	return 0
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
		bool ret = set< int >( pj, stepFinfo, 1 );
		assert( ret );
	}
}
