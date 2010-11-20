/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "MsgManager.h"
#include "SingleMsg.h"
#include "DiagonalMsg.h"
#include "OneToOneMsg.h"
#include "OneToAllMsg.h"
#include "AssignmentMsg.h"
#include "AssignVecMsg.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "Shell.h"
#include "Dinfo.h"
#include "Wildcard.h"

// Want to separate out this search path into the Makefile options
#include "../scheduling/Tick.h"
#include "../scheduling/TickMgr.h"
#include "../scheduling/TickPtr.h"
#include "../scheduling/Clock.h"

const unsigned int Shell::OkStatus = ~0;
const unsigned int Shell::ErrorStatus = ~1;
unsigned int Shell::numCores_;
unsigned int Shell::numNodes_;
unsigned int Shell::myNode_;
ProcInfo Shell::p_;

static SrcFinfo5< string, Id, Id, string, vector< unsigned int > > requestCreate( "requestCreate",
			"requestCreate( class, parent, newElm, name, dimensions ): "
			"creates a new Element on all nodes with the specified Id. "
			"Initiates a callback to indicate completion of operation. "
			"Goes to all nodes including self."
			);

static SrcFinfo2< unsigned int, unsigned int > ack( "ack",
			"ack( unsigned int node#, unsigned int status ):"
			"Acknowledges receipt and completion of a command on a worker node."
			"Goes back only to master node."
			);

static SrcFinfo1< Id  > requestDelete( "requestDelete",
			"requestDelete( doomedElement ):"
			"Deletes specified Element on all nodes."
			"Initiates a callback to indicate completion of operation."
			"Goes to all nodes including self." ); 

static SrcFinfo0 requestQuit( "requestQuit",
			"requestQuit():"
			"Emerges from the inner loop, and wraps up. No return value." );
static SrcFinfo1< double > requestStart( "requestStart",
			"requestStart( runtime ):"
			"Starts a simulation. Goes to all nodes including self."
			"Initiates a callback to indicate completion of run."
			);
static SrcFinfo1< unsigned int > requestStep( "requestStep",
			"requestStep():"
			"Advances a simulation for the specified # of steps."
			"Goes to all nodes including self."
			);
static SrcFinfo0 requestStop( "requestStop",
			"requestStop():"
			"Gently stops a simulation after completing current ops."
			"After this op it is save to do 'start' again, and it will"
			"resume where it left off"
			"Goes to all nodes including self."
			);
static SrcFinfo2< unsigned int, double > requestSetupTick( 
			"requestSetupTick",
			"requestSetupTick():"
			"Asks the Clock to coordinate the assignment of a specific"
			"clock tick. Args: Tick#, dt."
			"Goes to all nodes including self."
			);
static SrcFinfo0 requestReinit( "requestReinit",
			"requestReinit():"
			"Reinits a simulation: sets to time 0."
			"If simulation is running it stops it first."
			"Goes to all nodes including self."
			);

static SrcFinfo0 requestTerminate( "requestTerminate",
			"requestTerminate():"
			"Violently stops a simulation, possibly leaving things half-done."
			"Goes to all nodes including self."
			);
static SrcFinfo5< string, FullId, string, FullId, string > requestAddMsg( 
			"requestAddMsg",
			"requestAddMsg( type, src, srcField, dest, destField );"
			"Creates specified Msg between specified Element on all nodes."
			"Initiates a callback to indicate completion of operation."
			"Goes to all nodes including self."
			); 
static SrcFinfo4< Id, DataId, FuncId, PrepackedBuffer > requestSet(
			"requestSet",
			"requestSet( tgtId, tgtDataId, tgtFieldId, value ):"
			"Assigns a value on target field."
			);
static SrcFinfo2< Id, Id > requestMove(
			"move",
			"move( origId, newParent);"
			"Moves origId to become a child of newParent"
			);
static SrcFinfo4< vector< Id >, string, unsigned int, bool > requestCopy(
			"copy",
			"copy( origId, newParent, numRepeats, copyExtMsg );"
			"Copies origId to become a child of newParent"
			);
static SrcFinfo3< string, string, unsigned int > requestUseClock(
			"useClock",
			"useClock( path, field, tick# );"
			"Specifies which clock tick to use for all elements in Path."
			"The 'field' is typically process, but some cases need to send"
			"updates to the 'init' field."
			"Tick # specifies which tick to be attached to the objects."
			);

static DestFinfo handleUseClock( "handleUseClock", 
			"Deals with assignment of path to a given clock.",
			new EpFunc3< Shell, string, string, unsigned int >( 
				&Shell::handleUseClock )
			);

static DestFinfo handleCreate( "create", 
			"create( class, parent, newElm, name, dimensions )",
			new EpFunc5< Shell, string, Id, Id, string, vector< unsigned int > >( &Shell::handleCreate ) );

static DestFinfo del( "delete", 
			"Destroys Element, all its messages, and all its children. Args: Id",
			new EpFunc1< Shell, Id >( & Shell::destroy ) );

static DestFinfo handleAck( "handleAck", 
			"Keeps track of # of acks to a blocking shell command. Arg: Source node num.",
			new OpFunc2< Shell, unsigned int, unsigned int >( 
				& Shell::handleAck ) );

static DestFinfo handleQuit( "handleQuit", 
			"quit(): Quits simulation.",
			new OpFunc0< Shell >( & Shell::handleQuit ) );

static DestFinfo handleStart( "start", 
			"Starts off a simulation for the specified run time, automatically partitioning among threads if the settings are right",
			new OpFunc1< Shell, double >( & Shell::handleStart ) );
static DestFinfo handleReinit( "reinit", 
			"Reinitializes simulation, sets current time to zero, and puts all variables at initial conditions.",
			new OpFunc0< Shell >( & Shell::handleReinit ) );
static DestFinfo handleStop( "Stop", 
			"Cleanly stops simulation. Lets current timestep complete. Can resume cleanly.",
			new OpFunc0< Shell >( & Shell::handleStop ) );
static DestFinfo handleTerminate( "Terminate", 
			"Forcefully terminates simulation. Current timestep does not complete. Cannot resume cleanly.",
			new OpFunc0< Shell >( & Shell::handleTerminate ) );
			

static DestFinfo handleAddMsg( "handleAddMsg", 
			"Makes a msg",
			new EpFunc5< Shell, string, FullId, string, FullId, string >
				( & Shell::handleAddMsg ) );

static DestFinfo handleSet( "handleSet", 
			"Deals with request, to set specified field on any node to a value.",
			new OpFunc4< Shell, Id, DataId, FuncId, PrepackedBuffer >( 
				&Shell::handleSet )
			);

static SrcFinfo0 lowLevelSet(
			"lowLevelSet",
			"lowlevelSet():"
			"Low-level SrcFinfo. Not for external use, internally used as"
			"a handle to assign a value on target field."
);


/**
 * Sequence is:
 * innerDispatchGet->requestGet->handleGet->lowLevelGet->get_field->
 * 	receiveGet->completeGet
 */

static SrcFinfo3< Id, DataId, FuncId > requestGet( "requestGet",
			"Function to request another Element for a value" );

static DestFinfo handleGet( "handleGet", 
			"handleGet( Id elementId, DataId index, FuncId fid )"
			"Deals with requestGet, to get specified field from any node.",
			new OpFunc3< Shell, Id, DataId, FuncId >( 
				&Shell::handleGet )
			);
static SrcFinfo1< FuncId > lowLevelGet(
			"lowLevelGet",
			"lowlevelGet():"
			"Low-level SrcFinfo. Not for external use, internally used as"
			"a handle to request a value from target field."
);

// This function is called by directly inserting entries into the queue,
// when getting a value.
static DestFinfo lowLevelReceiveGet( "lowLevelReceiveGet", 
	"lowLevelReceiveGet( PrepackedBuffer data )"
	"Function on worker node Shell to handle the value returned by object.",
	new OpFunc1< Shell, PrepackedBuffer >( &Shell::lowLevelRecvGet )
);

static DestFinfo receiveGet( "receiveGet", 
	"receiveGet( Uint node#, Uint status, PrepackedBuffer data )"
	"Function on master shell that handles the value relayed from worker.",
	new OpFunc3< Shell, unsigned int, unsigned int, PrepackedBuffer >( &Shell::recvGet )
);

static SrcFinfo3< unsigned int, unsigned int, PrepackedBuffer > relayGet(
	"relayGet",
	"relayGet( node, status, data ): Passes 'get' data back to master node"
);

static DestFinfo handleMove( "move", 
		"handleMove( Id orig, Id newParent ): "
		"moves an Element to a new parent",
	new EpFunc2< Shell, Id, Id >( & Shell::handleMove ) );

static DestFinfo handleCopy( "handleCopy", 
		"handleCopy( vector< Id > args, string newName, unsigned int nCopies, bool copyExtMsgs ): "
		" The vector< Id > has Id orig, Id newParent, Id newElm. " 
		"This function copies an Element and all its children to a new parent."
		" May also expand out the original into nCopies copies."
		" Normally all messages within the copy tree are also copied. "
		" If the flag copyExtMsgs is true, then all msgs going out are also copied.",
			new EpFunc4< Shell, vector< Id >, string, unsigned int, bool >( 
				& Shell::handleCopy ) );

/*
static SrcFinfo3< unsigned int, double, unsigned int > requestSetClock(
			"requestSetClock",
			"requestSetClock( tickNum, dt, unsigned int stage )"
		);

static DestFinfo handleSetClock( "handleSetClock", 
			"handleSetClock( unsigned int tickNum, double dt, unsigned int stage )",
			new OpFunc3< Shell, unsigned int, double, unsigned int >(
				&shell::handleSetClock )
			);
			*/

static Finfo* shellMaster[] = {
	&requestCreate, &requestDelete, &requestQuit, 
	&requestStart, &requestReinit, &requestStop, &requestTerminate,
	&requestAddMsg, &requestSet, &requestGet, &receiveGet, 
	&requestMove, &requestCopy, &requestUseClock,
	&handleAck };
static Finfo* shellWorker[] = {
	&handleCreate, &del, &handleQuit, 
		&handleStart, &handleReinit, &handleStop, &handleTerminate,
		&handleAddMsg, &handleSet, &handleGet, &relayGet, 
		&handleMove, &handleCopy, &handleUseClock,
	&ack };

static Finfo* clockControlFinfos[] = 
{
	&requestStart, &requestStep, &requestStop, &requestSetupTick,
	&requestReinit, &handleAck
};

const Cinfo* Shell::initCinfo()
{
////////////////////////////////////////////////////////////////
// Value Finfos
////////////////////////////////////////////////////////////////
	static ValueFinfo< Shell, string > name( 
			"name",
			"Name of object", 
			&Shell::setName, 
			&Shell::getName );

	static ValueFinfo< Shell, bool > quit( 
			"quit",
			"Flag to tell the system to quit", 
			&Shell::setQuit, 
			&Shell::getQuit );

////////////////////////////////////////////////////////////////
// Dest Finfos: Functions handled by Shell
////////////////////////////////////////////////////////////////
		static DestFinfo setclock( "setclock", 
			"Assigns clock ticks. Args: tick#, dt",
			new OpFunc2< Shell, unsigned int, double >( & Shell::doSetClock ) );
		static DestFinfo loadBalance( "loadBalance", 
			"Set up load balancing",
			new OpFunc0< Shell >( & Shell::loadBalance ) );

		static SharedFinfo master( "master",
			"Issues commands from master shell to worker shells located "
			"on different nodes. Also handles acknowledgements from them.",
			shellMaster, sizeof( shellMaster ) / sizeof( const Finfo* )
		);
		static SharedFinfo worker( "worker",
			"Handles commands arriving from master shell on node 0."
			"Sends out acknowledgements from them.",
			shellWorker, sizeof( shellWorker ) / sizeof( const Finfo* )

		);
		static SharedFinfo clockControl( "clockControl",
			"Controls the system Clock",
			clockControlFinfos, 
			sizeof( clockControlFinfos ) / sizeof( const Finfo* )
		);
	
	static Finfo* shellFinfos[] = {
		&name,
		&quit,
		&lowLevelReceiveGet,
		&setclock,
		&loadBalance,

////////////////////////////////////////////////////////////////
//  Predefined Msg Src and MsgDests.
////////////////////////////////////////////////////////////////

		&requestGet,
		// &requestSet,
		&lowLevelSet,
		&lowLevelGet,
////////////////////////////////////////////////////////////////
//  Shared msg
////////////////////////////////////////////////////////////////
		&master,
		&worker,
		&clockControl,
	};

	static Cinfo shellCinfo (
		"Shell",
		Neutral::initCinfo(),
		shellFinfos,
		sizeof( shellFinfos ) / sizeof( Finfo* ),
		new Dinfo< Shell >()
	);

	return &shellCinfo;
}

static const Cinfo* shellCinfo = Shell::initCinfo();


Shell::Shell()
	: name_( "" ),
		quit_( 0 ), 
		isSingleThreaded_( 0 ),
		numAcks_( 0 ),
		acked_( 1, 0 ),
		barrier1_( 0 ),
		barrier2_( 0 ),
		isRunning_( 0 ),
		doReinit_( 0 ),
		runtime_( 0.0 ),
		cwe_( Id() )
{
	;
}

void Shell::setShellElement( Element* shelle )
{
	shelle_ = shelle;
}
////////////////////////////////////////////////////////////////
// Parser functions.
////////////////////////////////////////////////////////////////

/**
 * This is the version used by the parser. Acts as a blocking,
 * serial-like interface to a potentially multithread, multinode call.
 * Returns the new Id index.
 * The data of the new Element is not necessarily allocated at this point,
 * that can be deferred till the global Instantiate or Reset calls.
 * Idea is that the model should be fully defined before load balancing.
 *
 */
Id Shell::doCreate( string type, Id parent, string name, vector< unsigned int > dimensions )
{
	Id ret = Id::nextId();
	initAck(); // Nasty thread stuff happens here for multithread mode.
		requestCreate.send( Id().eref(), &p_, type, parent, ret, name, dimensions );
	waitForAck();
	return ret;
}

bool Shell::doDelete( Id i )
{
	initAck();
		requestDelete.send( Id().eref(), &p_, i );
	waitForAck();
	return 1;
}

MsgId Shell::doAddMsg( const string& msgType, 
	FullId src, const string& srcField, 
	FullId dest, const string& destField )
{
	if ( !src.id() ) {
		cout << myNode_ << ": Error: Shell::doAddMsg: src not found\n";
		return Msg::badMsg;
	}
	if ( !dest.id() ) {
		cout << myNode_ << ": Error: Shell::doAddMsg: dest not found\n";
		return Msg::badMsg;
	}
	const Finfo* f1 = src.id()->cinfo()->findFinfo( srcField );
	if ( !f1 ) {
		cout << myNode_ << ": Shell::doAddMsg: Error: Failed to find field " << srcField << 
			" on src: " << src.id()->getName() << "\n";
		return Msg::badMsg;
	}
	const Finfo* f2 = dest.id()->cinfo()->findFinfo( destField );
	if ( !f2 ) {
		cout << myNode_ << ": Shell::doAddMsg: Error: Failed to find field " << destField << 
			" on dest: " << dest.id()->getName() << "\n";
		return Msg::badMsg;
	}
	if ( ! f1->checkTarget( f2 ) ) {
		cout << myNode_ << ": Shell::doAddMsg: Error: Src/Dest Msg type mismatch: " << srcField << "/" << destField << endl;
		return Msg::badMsg;
	}
	initAck();
	requestAddMsg.send( Eref( shelle_, 0 ), &p_, 
		msgType, src, srcField, dest, destField );
	//	Qinfo::clearQ( &p_ );
	waitForAck();
	return latestMsgId_;
}

/**
 * Static function, sets up the master message that connects
 * all shells on all nodes to each other. Uses low-level calls to
 * do so.
 */
void Shell::connectMasterMsg()
{
	Id shellId;
	Element* shelle = shellId();
	const Finfo* f1 = shelle->cinfo()->findFinfo( "master" );
	if ( !f1 ) {
		cout << "Error: Shell::connectMasterMsg: failed to find 'master' msg\n";
		exit( 0 ); // Bad!
	}
	const Finfo* f2 = shelle->cinfo()->findFinfo( "worker" );
	if ( !f2 ) {
		cout << "Error: Shell::connectMasterMsg: failed to find 'worker' msg\n";
		exit( 0 ); // Bad!
	}

	Msg* m = 0;
		m = new OneToOneMsg( shelle, shelle );
	if ( m ) {
		if ( !f1->addMsg( f2, m->mid(), shelle ) ) {
			cout << "Error: failed in Shell::connectMasterMsg()\n";
			delete m; // Nasty, but rare.
		}
	}
	Id clockId( 1 );
	bool ret = innerAddMsg( "Single", FullId( shellId, 0 ), "clockControl", 
		FullId( clockId, 0 ), "clockControl" );
	assert( ret );
	// innerAddMsg( string msgType, FullId src, string srcField, FullId dest, string destField )
}

void Shell::doQuit( )
{
	initAck();
		requestQuit.send( Id().eref(), &p_ );
	waitForAck();
}

void Shell::doStart( double runtime )
{
	Eref sheller( shelle_, 0 );
	// Check if sim not yet initialized. Do it if needed.

	// Then actually run simulation.
	initAck();
		requestStart.send( sheller, &p_, runtime );
	waitForAck();
	// cout << Shell::myNode() << ": Shell::doStart(" << runtime << ")" << endl;
	// Qinfo::reportQ();
	// cout << myNode_ << ": Shell::doStart: quitting\n";
}

void Shell::doReinit()
{
	Eref sheller( shelle_, 0 );
	initAck();
		requestReinit.send( sheller, &p_ );
	waitForAck();
}

void Shell::doStop()
{
	Eref sheller( shelle_, 0 );
	initAck();
		requestStop.send( sheller, &p_ );
	waitForAck();
}
////////////////////////////////////////////////////////////////////////

void Shell::doSetClock( unsigned int tickNum, double dt )
{
	Eref ce = Id( 1 ).eref();
	assert( ce.element() );
	// We do NOT go through the message queuing here, as the clock is
	// always local and this operation fiddles with scheduling.

	Clock* clock = reinterpret_cast< Clock* >( ce.data() );
	clock->setupTick( tickNum, dt );
}

void Shell::doUseClock( string path, string field, unsigned int tick )
{
	Eref sheller( shelle_, 0 );
	initAck();
		requestUseClock.send( sheller, &p_, path, field, tick );
	waitForAck();
}

////////////////////////////////////////////////////////////////////////

void Shell::doMove( Id orig, Id newParent )
{
	if ( orig == Id() ) {
		cout << "Error: Shell::doMove: Cannot move root Element\n";
		return;
	}

	if ( newParent() == 0 ) {
		cout << "Error: Shell::doMove: Cannot move object to null parent \n";
		return;
	}
	if ( Neutral::isDescendant( newParent, orig ) ) {
		cout << "Error: Shell::doMove: Cannot move object to descendant in tree\n";
		return;
		
	}
	// Put in check here that newParent is not a child of orig.
	Eref sheller( shelle_, 0 );
	initAck();
		requestMove.send( sheller, &p_, orig, newParent );
	waitForAck();
}

/**
 * static func.
 * Chops up the names in the path into the vector of strings. 
 * Returns true if it starts at '/'.
 */
bool Shell::chopPath( const string& path, vector< string >& ret, 
	char separator )
{
	// /foo/bar/zod
	// foo/bar/zod
	// ./foo/bar/zod
	// ../foo/bar/zod
	// .
	// /
	// ..
	ret.resize( 0 );
	if ( path.length() == 0 )
		return 1; // Treat it as an absolute path

	bool isAbsolute = 0;
	string temp = path;
	if ( path[0] == separator ) {
		isAbsolute = 1;
		if ( path.length() == 1 )
			return 1;
		temp = temp.substr( 1 );
	}

	string::size_type pos = temp.find_first_of( separator );
	ret.push_back( temp.substr( 0, pos ) );
	while ( pos != string::npos ) {
		temp = temp.substr( pos + 1 );
		if ( temp.length() == 0 )
			break;
		pos = temp.find_first_of( separator );
		ret.push_back( temp.substr( 0, pos ) );
	}

	/*
	do {
		ret.push_back( temp.substr( 0, pos ) );
		temp = temp.substr( pos );
	} while ( pos != string::npos ) ;
	*/
	return isAbsolute;
}

/// non-static func. Returns the Id found by traversing the specified path.
Id Shell::doFind( const string& path ) const
{
	Id curr = Id();
	vector< string > names;
	bool isAbsolute = chopPath( path, names );

	if ( !isAbsolute )
		curr = cwe_;
	
	for ( vector< string >::iterator i = names.begin(); 
		i != names.end(); ++i ) {
		if ( *i == "." ) {
		} else if ( *i == ".." ) {
			curr = Neutral::parent( curr.eref() ).id;
		} else {
			curr = Neutral::child( curr.eref(), *i );
		}
	}
	return curr;
}

/// Static func.
void Shell::clearRestructuringQ()
{
	// cout << "o";
}
////////////////////////////////////////////////////////////////
// DestFuncs
////////////////////////////////////////////////////////////////

/**
 * The process call happens at a time when there are no more incoming
 * msgs to the Shell making their way through the message system.
 * However, there may be outgoing msgs queued up.
 */
void Shell::process( const Eref& e, ProcPtr p )
{
	Id clockId( 1 );
	Clock* clock = reinterpret_cast< Clock* >( clockId.eref().data() );
	Qinfo q;

	if ( isRunning_ ) {
		start( runtime_ ); // This is a blocking call
		if ( doReinit_ ) { // It may have been stopped by the reinit call
			clock->reinit( clockId.eref(), &q );
			doReinit_ = 0;
		}
		ack.send( Eref( shelle_, 0 ), &p_, myNode_, OkStatus );
		return;
	} else if ( doReinit_ ) { 
		isRunning_ = 0;
		clock->reinit( clockId.eref(), &q );
		doReinit_ = 0;
		ack.send( Eref( shelle_, 0 ), &p_, myNode_, OkStatus );
	}
}


void Shell::setName( string name )
{
	name_ = name;
}

string Shell::getName() const
{
	return name_;
}

void Shell::setCwe( Id val )
{
	cwe_ = val;
}

Id Shell::getCwe() const
{
	return cwe_;
}

void Shell::setQuit( bool val )
{
	quit_ = val;
}

bool Shell::getQuit() const
{
	return quit_;
}

void Shell::handleQuit()
{
	quit_ = 1;
}

const char* Shell::getBuf() const
{
	if ( getBuf_.size() > 0 )
		return &( getBuf_[0] );
	return 0;
}

void Shell::handleStart( double runtime )
{
	/*
	isRunning_ = 1;
	runtime_ = runtime;
	// The actual start operation is handed over to the process stage
	// to avoid confusing the queue.
	// start( runtime );
	// ack.send( Eref( shelle_, 0 ), &p_, myNode_, OkStatus );
	*/
}

void Shell::handleReinit( )
{
	/*
	Id clockId( 1 );
	Clock* clock = reinterpret_cast< Clock* >( clockId.eref().data() );
	Qinfo q;
	clock->stop( clockId.eref(), &q );
	*/
	/*
	doReinit_ = 1;
	handleStop();
	*/
	// Hand over the reinit operation to the Shell::process function,
	// to avoid confusing the queue.
}

void Shell::handleStop()
{
/*
	Id clockId( 1 );
	Clock* clock = reinterpret_cast< Clock* >( clockId.eref().data() );
	Qinfo q;
	isRunning_ = 0;
	clock->stop( clockId.eref(), &q );
	*/
}

void Shell::handleTerminate()
{
	/*
	Id clockId( 1 );
	Clock* clock = reinterpret_cast< Clock* >( clockId.eref().data() );
	Qinfo q;
	isRunning_ = 0;
	clock->terminate( clockId.eref(), &q );
	*/
}


/**
 * This function handles the message request to create an Element.
 * This request specifies the Id of the new Element and is handled on
 * all nodes.
 *
 * In due course we also have to set up the node decomposition of the
 * Element, but for now the num indicates the total # of array entries.
 * This gets a bit complicated if the Element is a multidim array.
 */
void Shell::handleCreate( const Eref& e, const Qinfo* q, 
	string type, Id parent, Id newElm, string name,
	vector< unsigned int > dimensions )
{
	if ( q->addToStructuralQ() )
		return;
	// cout << myNode_ << ": In Shell::handleCreate for element " << name << " id " << newElm << ", dim = " << dimensions[0] << endl;
	innerCreate( type, parent, newElm, name, dimensions );
	// cout << myNode_ << ": Shell::handleCreate inner Create done for element " << name << " id " << newElm << endl;
//	if ( myNode_ != 0 )
	ack.send( e, &p_, Shell::myNode(), OkStatus );
	// cout << myNode_ << ": Shell::handleCreate ack sent" << endl;
}

/// Utility function 
bool Shell::adopt( Id parent, Id child ) {
	static const Finfo* pf = Neutral::initCinfo()->findFinfo( "parentMsg" );
	// static const DestFinfo* pf2 = dynamic_cast< const DestFinfo* >( pf );
	// static const FuncId pafid = pf2->getFid();
	static const Finfo* f1 = Neutral::initCinfo()->findFinfo( "childMsg" );

	assert( !( child() == 0 ) );
	assert( !( child == Id() ) );
	assert( !( parent() == 0 ) );

	Msg* m = new OneToAllMsg( parent.eref(), child() );
	assert( m );
	if ( !f1->addMsg( pf, m->mid(), parent() ) ) {
		cout << "move: Error: unable to add parent->child msg from " <<
			parent()->getName() << " to " << child()->getName() << "\n";
		return 0;
	}
	child()->setGroup( parent()->getGroup() );
	return 1;
}

/**
 * This function actually creates the object.
 */
void Shell::innerCreate( string type, Id parent, Id newElm, string name,
	const vector< unsigned int >& dimensions )
{
	const Cinfo* c = Cinfo::find( type );
	if ( c ) {
		Element* pa = parent();
		if ( !pa ) {
			stringstream ss;
			ss << "innerCreate: Parent Element'" << parent << "' not found. No Element created";
			warning( ss.str() );
			return;
		}
		/*
		const Finfo* f1 = pa->cinfo()->findFinfo( "childMsg" );
		if ( !f1 ) {
			stringstream ss;
			ss << "innerCreate: Parent Element'" << parent << "' cannot handle children. No Element created";
			warning( ss.str() );
			return;
		}
		const Finfo* f2 = Neutral::initCinfo()->findFinfo( "parentMsg" );
		assert( f2 );
		// cout << myNode_ << ": Shell::innerCreate newElmId= " << newElm << endl;
		*/
		Element* ret = new Element( newElm, c, name, dimensions );
		assert( ret );
		adopt( parent, newElm );

		/*
		Msg* m = new OneToAllMsg( parent.eref(), ret );
		assert( m );
		if ( !f1->addMsg( f2, m->mid(), parent() ) ) {
			cout << "innerCreate: Error: unable to add parent->child msg for " <<
				name << "\n";
			return;
		}
		*/

	} else {
		stringstream ss;
		ss << "innerCreate: Class '" << type << "' not known. No Element created";
		warning( ss.str() );
	}
}

void Shell::destroy( const Eref& e, const Qinfo* q, Id eid)
{
	if ( q->addToStructuralQ() )
		return;

	Neutral *n = reinterpret_cast< Neutral* >( e.data() );
	assert( n );
	n->destroy( eid.eref(), 0, 0 );
	// eid.destroy();
	// cout << myNode_ << ": Shell::destroy done for element id " << eid << endl;

	ack.send( e, &p_, Shell::myNode(), OkStatus );
}


/**
 * Wrapper function, that does the ack. Other functions also use the
 * inner function to build message trees, so we don't want it to emit
 * multiple acks.
 */
void Shell::handleAddMsg( const Eref& e, const Qinfo* q,
	string msgType, FullId src, string srcField, 
	FullId dest, string destField )
{
	if ( q->addToStructuralQ() )
		return;
	if ( innerAddMsg( msgType, src, srcField, dest, destField ) )
		ack.send( Eref( shelle_, 0 ), &p_, Shell::myNode(), OkStatus );
	else
		ack.send( Eref( shelle_, 0), &p_, Shell::myNode(), ErrorStatus );
}

/**
 * The actual function that adds messages. Does NOT send an ack.
 */
bool Shell::innerAddMsg( string msgType, FullId src, string srcField, 
	FullId dest, string destField )
{
	// cout << myNode_ << ", Shell::handleAddMsg" << "\n";
	const Finfo* f1 = src.id()->cinfo()->findFinfo( srcField );
	if ( f1 == 0 ) return 0;
	// assert( f1 != 0 );
	const Finfo* f2 = dest.id()->cinfo()->findFinfo( destField );
	if ( f2 == 0 ) return 0;
	// assert( f2 != 0 );
	
	// Should have been done before msgs request went out.
	assert( f1->checkTarget( f2 ) );

	latestMsgId_ = Msg::badMsg;

	Msg *m = 0;
	if ( msgType == "diagonal" || msgType == "Diagonal" ) {
		m = new DiagonalMsg( src.id(), dest.id() );
	} else if ( msgType == "sparse" || msgType == "Sparse" ) {
		m = new SparseMsg( src.id(), dest.id() );
	} else if ( msgType == "Single" || msgType == "single" ) {
		m = new SingleMsg( src.eref(), dest.eref() );
	} else if ( msgType == "OneToAll" || msgType == "oneToAll" ) {
		m = new OneToAllMsg( src.eref(), dest.id() );
	} else if ( msgType == "OneToOne" || msgType == "oneToOne" ) {
		m = new OneToOneMsg( src.id(), dest.id() );
	} else {
		cout << myNode_ << 
			": Error: Shell::handleAddMsg: msgType not known: "
			<< msgType << endl;
		// ack.send( Eref( shelle_, 0 ), &p_, Shell::myNode(), ErrorStatus );
		return 0;
	}
	if ( m ) {
		if ( f1->addMsg( f2, m->mid(), src.id() ) ) {
			latestMsgId_ = m->mid();
			// ack.send( Eref( shelle_, 0 ), &p_, Shell::myNode(), OkStatus );
			return 1;
		}
		delete m;
	}
	cout << myNode_ << 
			": Error: Shell::handleAddMsg: Unable to make/connect Msg: "
			<< msgType << " from " << src.id()->getName() <<
			" to " << dest.id()->getName() << endl;
	return 1;
//	ack.send( Eref( shelle_, 0 ), &p_, Shell::myNode(), ErrorStatus );
}

void Shell::handleMove( const Eref& e, const Qinfo* q,
	Id orig, Id newParent )
{
	static const Finfo* pf = Neutral::initCinfo()->findFinfo( "parentMsg" );
	static const DestFinfo* pf2 = dynamic_cast< const DestFinfo* >( pf );
	static const FuncId pafid = pf2->getFid();
	static const Finfo* f1 = Neutral::initCinfo()->findFinfo( "childMsg" );

	assert( !( orig == Id() ) );
	assert( !( newParent() == 0 ) );

	if ( q->addToStructuralQ() )
		return;

	MsgId mid = orig()->findCaller( pafid );
	Msg::deleteMsg( mid );

	Msg* m = new OneToAllMsg( newParent.eref(), orig() );
	assert( m );
	if ( !f1->addMsg( pf, m->mid(), newParent() ) ) {
		cout << "move: Error: unable to add parent->child msg from " <<
			newParent()->getName() << " to " << orig()->getName() << "\n";
		return;
	}
	
	ack.send( Eref( shelle_, 0 ), &p_, Shell::myNode(), OkStatus );
}

void Shell::handleUseClock( const Eref& e, const Qinfo* q,
	string path, string field, unsigned int tick)
{
	if ( q->addToStructuralQ() )
		return;
	vector< Id > list;
	wildcard( path, list ); // By default scans only Elements.
	string tickField = "process";
	if ( field.substr( 0, 4 ) == "proc" || field.substr( 0, 4 ) == "Proc" )
		field = tickField = "proc"; // Use the shared Msg with process and reinit.
	for ( vector< Id >::iterator i = list.begin(); i != list.end(); ++i ) {
		stringstream ss;
		FullId tickId( Id( 2 ), DataId( 0, tick ) );
		ss << tickField << tick;
		// bool ret = 
			innerAddMsg( "OneToAll", tickId, ss.str(), FullId( *i, 0 ), 
			field);
		// We just skip messages that don't work.
		/*
		if ( !ret ) {
			cout << Shell::myNode() << "Error: Shell::handleUseClock: Messaging failed\n";
			ack.send( Eref( shelle_, 0 ), &p_, Shell::myNode(), 
				ErrorStatus );
			return;
		}
		*/
	}
	ack.send( Eref( shelle_, 0 ), &p_, Shell::myNode(), OkStatus );
}

void Shell::warning( const string& text )
{
	cout << "Warning: Shell:: " << text << endl;
}

void Shell::error( const string& text )
{
	cout << "Error: Shell:: " << text << endl;
}

void Shell::wildcard( const string& path, vector< Id >& list )
{
	wildcardFind( path, list );
}

///////////////////////////////////////////////////////////////////////////
// Functions for handling acks for blocking Shell function calls.
///////////////////////////////////////////////////////////////////////////

/**
 * Generic handler for ack msgs from various nodes. Keeps track of
 * which nodes have responded.
 * This has been moved to ShellThreads.cpp, because that is 
 * where the other ack related functions live.
void Shell::handleAck( unsigned int ackNode, unsigned int status )
{
	assert( ackNode < numNodes_ );
	acked_[ ackNode ] = status;
		// Here we could also check which node(s) are last, in order to do
		// some dynamic load balancing.
	++numAcks_;
	if ( status != OkStatus ) {
		cout << myNode_ << ": Shell::handleAck: Error: status = " <<
			status << " from node " << ackNode << endl;
	}
}
 */

////////////////////////////////////////////////////////////////////////
// Some static utility functions
////////////////////////////////////////////////////////////////////////

// Statid func for returning the pet ProcInfo of the shell.
const ProcInfo* Shell::procInfo()
{
	return &p_;
}

/**
 * Static global, returns contents of shell buffer.
 */
const char* Shell::buf() 
{
	static Id shellid;
	static Element* shell = shellid();
	assert( shell );
	return (reinterpret_cast< Shell* >(shell->dataHandler()->data( 0 )) )->getBuf();
}


////////////////////////////////////////////////////////////////////////
// Functions for handling field set/get and func calls
////////////////////////////////////////////////////////////////////////

void Shell::innerSetVec( const Eref& er, FuncId fid, const PrepackedBuffer& arg )
{
	shelle_->clearBinding ( lowLevelGet.getBindIndex() );
	Msg* m = new AssignVecMsg( Eref( shelle_, 0 ), er.element(), Msg::setMsg );
	shelle_->addMsgAndFunc( m->mid(), fid, lowLevelGet.getBindIndex() );
	char* temp = new char[ arg.size() ];
	arg.conv2buf( temp );

	// Qinfo( funcId, srcIndex, size, useSendTo )
	Qinfo q( fid, 0, arg.size(), 0 );
	shelle_->asend( q, lowLevelGet.getBindIndex(), &p_, temp );

	delete[] temp;
}

void Shell::innerSet( const Eref& er, FuncId fid, const char* args, 
	unsigned int size )
{
	if ( er.isDataHere() ) {
		shelle_->clearBinding ( lowLevelGet.getBindIndex() );
		Msg* m = new AssignmentMsg( Eref( shelle_, 0 ), er, Msg::setMsg );
		shelle_->addMsgAndFunc( m->mid(), fid, lowLevelGet.getBindIndex() );
	
		// Qinfo( funcId, srcIndex, size, useSendTo )
		Qinfo q( fid, 0, size, 0 ); // Shell has index 0 for now.
		shelle_->asend( q, lowLevelGet.getBindIndex(), &p_, args );
	}
}

void Shell::handleSet( Id id, DataId d, FuncId fid, PrepackedBuffer arg )
{
	Eref er( id(), d );
	if ( arg.isVector() ) {
		innerSetVec( er, fid, arg );
	} else {
		innerSet( er, fid, arg.data(), arg.dataSize() );
	}
	ack.send( Eref( shelle_, 0 ), &p_, Shell::myNode(), OkStatus );
	// We assume that the ack will get back to the master node no sooner
	// than the field assignment. This is probably pretty safe. More to the
	// point, the Parser thread won't be able to do anything else before
	// the field assignment is done.
}

// Static function, used for developer-code triggered SetGet functions.
// Should only be issued from master node.
// This is a blocking function, and returns only when the job is done.
// mode = 0 is single value set, mode = 1 is vector set, mode = 2 is
// to set the entire target array to a single value.
void Shell::dispatchSet( const Eref& tgt, FuncId fid, const char* args,
	unsigned int size )
{
	Eref sheller = Id().eref();
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	if ( s->isSingleThreaded_ ) {
		s->innerSet( tgt, fid, args, size );
		Qinfo::clearQ( s->procInfo() );
	} else {
		PrepackedBuffer buf( args, size );
		s->innerDispatchSet( sheller, tgt, fid, buf );
	}
}

// regular function, does the actual dispatching.
void Shell::innerDispatchSet( Eref& sheller, const Eref& tgt, 
	FuncId fid, const PrepackedBuffer& buf )
{
	Id tgtId( tgt.element()->id() );
	initAck();
		requestSet.send( sheller, &p_,  tgtId, tgt.index(), fid, buf );
	waitForAck();
}

// Static function.
void Shell::dispatchSetVec( const Eref& tgt, FuncId fid, 
	const PrepackedBuffer& pb )
{
	Eref sheller = Id().eref();
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	if ( s->isSingleThreaded_ ) {
		s->innerSetVec( tgt, fid, pb );
		Qinfo::clearQ( s->procInfo() );
	} else {
		s->innerDispatchSet( sheller, tgt, fid, pb );
	}
}

/**
 * Returns buffer containing desired data.
 * Static function, used for developer-code triggered SetGet functions.
 * Should only be issued from master node.
 * This is a blocking function and returns only when the job is done.
 */
const char* Shell::dispatchGet( const Eref& e, const string& field, 
	const SetGet* sg )
{
	Eref tgt( e );
	const Finfo* gf = tgt.element()->cinfo()->findFinfo( field );
	if ( !gf ) {	// Could be a child Element. Field name changes.
		string f2 = field.substr( 4 );
		Id child = Neutral::child( tgt, f2 );
		if ( child == Id() ) {
			cout << myNode() << 
				": Error: Shell::dispatchGet: No field or child named '" <<
				field << "' was found\n";
		} else {
			gf = child()->cinfo()->findFinfo( "get_this" );
			assert( gf ); // Neutral has get_this, so all derived should too
			if ( child()->dataHandler()->totalEntries() ==
				e.element()->dataHandler()->totalEntries() )
				tgt = Eref( child(), e.index() );
			else if ( child()->dataHandler()->totalEntries() <= 1 )
				tgt = Eref( child(), 0 );
			else {
				cout << myNode() << 
					": Error: Shell::dispatchGet: child index mismatch\n";
				return 0;
			}
		}
	}
	const DestFinfo * df = dynamic_cast< const DestFinfo* >( gf );
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	if ( !df ) {
		cout << s->myNode() << ": Error: Shell::dispatchGet: field '" << field << "' not found on " << tgt << endl;
		return 0;
	}

	if ( df->getOpFunc()->checkSet( sg ) ) { // Type validation
		if ( s->isSingleThreaded_ ) {
			s->handleGet( tgt.id(), tgt.index(), df->getFid() );
			Qinfo::clearQ( s->procInfo() ); // phase 1: request to tgt elem
			Qinfo::clearQ( s->procInfo() ); // phase 2: response to relay
			Qinfo::clearQ( s->procInfo() ); // phase 3: data arrives.
			return s->getBuf();
		} else {
			Eref sheller = Id().eref();
			return s->innerDispatchGet( sheller, tgt, df->getFid() );
		}
	} else {
		cout << s->myNode() << ": Error: Shell::dispatchGet: type mismatch for field " << field << " on " << tgt << endl;
	}
	return 0;
}

/**
 * Tells all nodes to dig up specified field, if object is present on node.
 * Not thread safe: this should only run on master node.
 */
const char* Shell::innerDispatchGet( const Eref& sheller, const Eref& tgt, 
	FuncId fid )
{
	initAck();
		requestGet.send( sheller, &p_, tgt.element()->id(), tgt.index(), fid );
	waitForAck();
	return &getBuf_[0];
}


/**
 * This operates on the worker node. It handles the Get request from
 * the master node, and dispatches if need to the local object.
 */
void Shell::handleGet( Id id, DataId index, FuncId fid )
{
	Eref sheller( shelle_, 0 );
	Eref tgt( id(), index );
	if ( id()->dataHandler()->isDataHere( index ) ) {
		shelle_->clearBinding( lowLevelGet.getBindIndex() );
		Msg* m = new AssignmentMsg( sheller, tgt, Msg::setMsg );
		shelle_->addMsgAndFunc( m->mid(), fid, lowLevelGet.getBindIndex() );
		FuncId retFunc = lowLevelReceiveGet.getFid();
		lowLevelGet.send( sheller, &p_, retFunc );
	} else {
		ack.send( sheller, &p_, myNode_, OkStatus );
	}
}

void Shell::recvGet( 
	unsigned int node, unsigned int status, PrepackedBuffer pb )
{
	if ( myNode_ == 0 ) {
		getBuf_.resize( pb.dataSize() );
		memcpy( &getBuf_[0], pb.data(), pb.dataSize() );
		handleAck( node, status );
	} else {
		// cout << myNode_ << ": Error: Shell::recvGet: should never be called except on node 0\n";
	}
}

void Shell::lowLevelRecvGet( PrepackedBuffer pb )
{
	relayGet.send( Eref( shelle_, 0 ), &p_, myNode(), OkStatus, pb );
}

////////////////////////////////////////////////////////////////////////
