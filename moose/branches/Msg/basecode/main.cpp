/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include <sys/time.h>
#include <math.h>
#include <queue>
#include <unistd.h> // for getopt
#include "../scheduling/Tick.h"
#include "../scheduling/TickMgr.h"
#include "../scheduling/TickPtr.h"
#include "../scheduling/Clock.h"
#include "DiagonalMsg.h"
#include "MsgManager.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "AssignmentMsg.h"
#include "AssignVecMsg.h"
#ifdef USE_MPI
#include <mpi.h>
#endif

extern void testSync();
extern void testAsync();
extern void testSyncArray( unsigned int size, unsigned int numThreads,
	unsigned int method );
extern void testShell();
extern void testScheduling();
extern void testSchedulingProcess();
extern void testBuiltins();
extern void testBuiltinsProcess();

extern void testMpiScheduling();
extern void testMpiBuiltins();
extern void testMpiShell();
extern void testMsg();
extern void testMpiMsg();
extern void testKinetics();

extern void initMsgManagers();
extern void destroyMsgManagers();
extern void speedTestMultiNodeIntFireNetwork( 
	unsigned int size, unsigned int runsteps );
#ifdef DO_UNIT_TESTS
void regressionTests();
#endif
bool benchmarkTests( int argc, char** argv );

//////////////////////////////////////////////////////////////////
// System-dependent function here
//////////////////////////////////////////////////////////////////

unsigned int getNumCores()
{
	unsigned int numCPU = 1;
#ifdef WIN_32
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );

	numCPU = sysinfo.dwNumberOfProcessors;
#endif

#ifdef LINUX
	numCPU = sysconf( _SC_NPROCESSORS_ONLN );
#endif

#ifdef MACOSX
	nt mib[4];
	size_t len; 

	/* set the mib for hw.ncpu */
	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

	/* get the number of CPUs from the system */
	sysctl(mib, 2, &numCPU, &len, NULL, 0);

	if( numCPU < 1 ) 
	{
		mib[1] = HW_NCPU;
		sysctl( mib, 2, &numCPU, &len, NULL, 0 );
		if( numCPU < 1 )
		{
		numCPU = 1;
		}
	}
#endif
	return numCPU;
}

//////////////////////////////////////////////////////////////////

Id init( int argc, char** argv )
{
	int numCores = getNumCores();
	int numNodes = 1;
	int myNode = 0;
	bool isSingleThreaded = 0;
	bool isInfinite = 0;
	int opt;
#ifdef USE_MPI
	int provided;
	// OpenMPI does not use argc or argv.
	// unsigned int temp_argc = 1;
	//MPI_Init_thread( &temp_argc, &argv, MPI_THREAD_SERIALIZED, &provided );
	MPI_Init_thread( &argc, &argv, MPI_THREAD_SERIALIZED, &provided );

	MPI_Comm_size( MPI_COMM_WORLD, &numNodes );
	MPI_Comm_rank( MPI_COMM_WORLD, &myNode );
	if ( provided < MPI_THREAD_SERIALIZED && myNode == 0 ) {
		cout << "Warning: This MPI implementation does not like multithreading: " << provided << "\n";
	}
	// myNode = MPI::COMM_WORLD.Get_rank();
#endif
	/**
	 * Here we allow the user to override the automatic identification
	 * of processor configuration
	 */
	while ( ( opt = getopt( argc, argv, "shin:c:b:B:" ) ) != -1 ) {
		switch ( opt ) {
			case 's': // Single threaded mode
				isSingleThreaded = 1;
				break;
			case 'i' : // infinite loop, used for multinode debugging, to give gdb something to attach to.
				isInfinite = 1;
				break;
			case 'n': // Multiple nodes
				numNodes = atoi( optarg );
				break;
			case 'c': // Multiple cores per node
				// Each node handles 
				numCores = atoi( optarg );
				break;
			case 'b': // Benchmark: handle later.
				break;
			case 'B': // Benchmark, dump data: handle later.
				break;
			case 'h': // help
			default:
				cout << "Usage: moose -singleThreaded -help -infiniteLoop -c numCores -n numNodes -benchmark [ksolve intFire]\n";
				exit( 1 );
		}
	}
	cout << "on node " << myNode << ", numNodes = " << numNodes << ", numCores = " << numCores << endl;

	Msg::initNull();
	Id shellId;
	vector< unsigned int > dims;
	dims.push_back( 1 );
	Element* shelle = 
		new Element( shellId, Shell::initCinfo(), "root", dims, 1 );
	// Shell::initCinfo()->create( shellId, "root", 1 );

	Id clockId = Id::nextId();
	// Clock::initCinfo()->create( clockId, "clock", 1 );
	// Element* clocke = 
		new Element( clockId, Clock::initCinfo(), "clock", dims, 1 );
	// Clock::initCinfo()->postCreationFunc( clockId, clocke );
	// Should put this initialization stuff within the Clock creation
	// step. This means I need to add an optional init func into the Cinfo
	// constructor, or to add the init func as a virtual func in Data.
	Id tickId( 2 );
	assert( tickId() != 0 );
	assert( tickId()->getName() == "tick" ) ;

	assert ( shellId == Id() );
	assert( clockId == Id( 1 ) );
	assert( tickId == Id( 2 ) );

	initMsgManagers();

	// SetGet::setShell();
	Shell* s = reinterpret_cast< Shell* >( shellId.eref().data() );
	s->setShellElement( shelle );
	s->setHardware( isSingleThreaded, numCores, numNodes, myNode );
	s->loadBalance();
	Shell::connectMasterMsg();
	// Msg* m = new OneToOneMsg( shelle, shelle );
	// assert ( m != 0 );
	
	while ( isInfinite ) // busy loop for debugging under gdb and MPI.
		;

	return shellId;
}

/**
 * These tests are meant to run on individual nodes, and should
 * not invoke MPI calls. They should not be run when MPI is running.
 * These tests do not use the threaded/MPI event loop and are the most
 * basic of the set.
 */
void nonMpiTests( Shell* s )
{
#ifdef DO_UNIT_TESTS
	if ( Shell::myNode() == 0 ) {
		unsigned int numNodes = s->numNodes();
		unsigned int numCores = s->numCores();
		if ( numCores > 0 )
		// s->setHardware( isSingleThreaded, numCores, numNodes, myNode );
		s->setHardware( 1, 1, 1, 0 );
		testAsync();
		testMsg();
		testShell();
		testScheduling();
		testBuiltins();
		testKinetics();
		s->setHardware( (numCores == 1), numCores, numNodes, 0 );
	}
#endif
}

/**
 * These tests involve the threaded/MPI process loop and are the next
 * level of tests.
 */
void processTests( Shell* s )
{
	testSchedulingProcess();
	testBuiltinsProcess();
}

/**
 * These are tests that are MPI safe. They should also run
 * properly on single nodes.
 */
void mpiTests()
{
#ifdef DO_UNIT_TESTS
		testMpiMsg();
		testMpiShell();
		testMpiBuiltins();
		testMpiScheduling();
#endif
}

int main( int argc, char** argv )
{
	Id shellId = init( argc, argv );
	// Note that the main loop remains the parser loop, though it may
	// spawn a lot of other stuff.
	Element* shelle = shellId();
	Shell* s = reinterpret_cast< Shell* >( shelle->dataHandler()->data( 0 ) );
	nonMpiTests( s ); // These tests do not need the process loop.

	/*
	s->launchThreads(); // Here we set off the thread/MPI process loop.
	if ( s->myNode() == 0 ) {
		mpiTests();
#ifdef DO_UNIT_TESTS
		regressionTests();
#endif
		if ( benchmarkTests( argc, argv ) )
		s->doQuit();
	}
	
	// Somehow we need to return control to our parser. Then we clean up
	s->joinThreads();
	*/

	shellId.destroy();
	Id(1).destroy();
	Id(2).destroy();
	destroyMsgManagers();
#ifdef USE_MPI
	MPI_Finalize();
#endif
	return 0;
}

