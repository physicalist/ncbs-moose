/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifdef USE_MPI
#include <math.h>
#include "moose.h"
#include <mpi.h>
#include "PostMaster.h"
#include "ProxyElement.h"
#include "AsyncDestFinfo.h"
#include <sstream>
// #include <typeinfo>
// #include "../element/Neutral.h" // Possibly only need this for testing.
// #include "../shell/Shell.h" // Possibly only need this for testing.

#define DATA_TAG 0

const Cinfo* initPostMasterCinfo()
{
	/**
	 * This shared message communicates between the ParTick and 
	 * the PostMaster
	 */
	static Finfo* parShared[] = 
	{
		// This first entry is to tell the PostMaster to post iRecvs
		// The argument is the ordinal number of the clock tick
		new DestFinfo( "postIrecv", Ftype1< int >::global(), 
			RFCAST( &PostMaster::postIrecv ) ),
		// The second entry is to tell the PostMaster to post 'send'
		new DestFinfo( "postSend", Ftype1< int >::global(), 
			RFCAST( &PostMaster::postSend ) ),
		// The third entry is for polling the receipt of incoming data.
		// Each PostMaster does an MPI_Test on the earlier posted iRecv.
		new DestFinfo( "poll", Ftype1< int >::global(), 
			RFCAST( &PostMaster::poll ) ),
		// The fourth entry is for harvesting the poll request.
		// The argument is the node number handled by the postmaster.
		// It comes back when the polling on that postmaster is done.
		new SrcFinfo( "harvestPoll", Ftype1< unsigned int >::global() ),

		// The last entry tells targets to execute a Barrier command,
		// in order to synchronize all nodes.
		new DestFinfo( "barrier", Ftype0::global(), 
			RFCAST( &PostMaster::barrier ) ),
	};

	static Finfo* serialShared[] =
	{
		new SrcFinfo( "rawAdd", // addmsg using serialized data.
			Ftype1< string >::global() ),
		new SrcFinfo( "rawCopy", // copy using serialized data.
			Ftype1< string >::global() ),
		new SrcFinfo( "rawTest", // copy using serialized data.
			Ftype1< string >::global() ),
	};

	static Finfo* postMasterFinfos[] = 
	{
		new ValueFinfo( "localNode", 
					ValueFtype1< unsigned int >::global(),
					GFCAST( &PostMaster::getMyNode ), &dummyFunc 
		),
		new ValueFinfo( "remoteNode", 
					ValueFtype1< unsigned int >::global(),
					GFCAST( &PostMaster::getRemoteNode ),
					RFCAST( &PostMaster::setRemoteNode )
		),
		
		new ValueFinfo( "shellProxy", 
					ValueFtype1< Id >::global(),
					GFCAST( &PostMaster::getShellProxy ),
					RFCAST( &PostMaster::setShellProxy )
		),

		// derived from DestFinfo, main difference is it accepts all
		// kinds of incoming data.
		new AsyncDestFinfo( "async", 
			Ftype2< char*, unsigned int >::global(), 
			RFCAST( &PostMaster::async ) ),

		////////////////////////////////////////////////////////////////
		//	Shared messages.
		////////////////////////////////////////////////////////////////
		new SharedFinfo( "parTick", parShared, 
			sizeof( parShared ) / sizeof( Finfo* ) ),
		new SharedFinfo( "serial", serialShared, 
			sizeof( serialShared ) / sizeof( Finfo* ) ),
	};

	static Cinfo postMasterCinfo(
				"PostMaster",
				"Upi Bhalla",
				"PostMaster object. Manages parallel communications.",
				initNeutralCinfo(),
				postMasterFinfos,
				sizeof( postMasterFinfos ) / sizeof( Finfo* ),
				ValueFtype1< PostMaster >::global()
	);

	return &postMasterCinfo;
}

static const Cinfo* postMasterCinfo = initPostMasterCinfo();

static const Slot pollSlot = 
	initPostMasterCinfo()->getSlot( "parTick.harvestPoll" );
static const Slot addSlot = 
	initPostMasterCinfo()->getSlot( "serial.rawAdd" );
static const Slot copySlot = 
	initPostMasterCinfo()->getSlot( "serial.rawCopy" );
static const Slot testSlot = 
	initPostMasterCinfo()->getSlot( "serial.rawTest" );

//////////////////////////////////////////////////////////////////
// Here are the PostMaster class functions.
//////////////////////////////////////////////////////////////////
PostMaster::PostMaster()
	: remoteNode_( 0 ), 
	sendBuf_( 1000, 0 ), 
	sendBufPos_( sizeof( unsigned int ) ), 
	numSendBufMsgs_( 0 ), 
	recvBuf_( 1000, 0 ), 
	donePoll_( 0 ), 
	shellProxy_(), // Initializes to a null Id.
	comm_( &MPI::COMM_WORLD )
{
	localNode_ = MPI::COMM_WORLD.Get_rank(); 
	request_ = 0;
}

//////////////////////////////////////////////////////////////////
// Here we put the PostMaster Moose functions.
//////////////////////////////////////////////////////////////////

unsigned int PostMaster::getMyNode( Eref e )
{
		return static_cast< PostMaster* >( e.data() )->localNode_;
}

unsigned int PostMaster::getRemoteNode( Eref e )
{
		return static_cast< PostMaster* >( e.data() )->remoteNode_;
}

void PostMaster::setRemoteNode( const Conn* c, unsigned int node )
{
		static_cast< PostMaster* >( c->data() )->remoteNode_ = node;
}

Id PostMaster::getShellProxy( Eref e )
{
		return static_cast< PostMaster* >( e.data() )->shellProxy_;
}

void PostMaster::setShellProxy( const Conn* c, Id value )
{
		static_cast< PostMaster* >( c->data() )->shellProxy_ = value;
}

/////////////////////////////////////////////////////////////////////
// Stuff data into async buffer. Used mostly for testing.
/////////////////////////////////////////////////////////////////////
void PostMaster::async( const Conn* c, char* data, unsigned int size )
{
	PostMaster* pm = static_cast< PostMaster* >( c->data() );
	assert( pm != 0 );
	void* addr = pm->innerGetAsyncParBuf( c, size );
	memcpy( addr, data, size );
}

/////////////////////////////////////////////////////////////////////
// Here we handle passing messages to off-nodes
/////////////////////////////////////////////////////////////////////


/**
 * Manages the setup of a bidirectional shared message emanating from 
 * this postmaster to one or more targets. 
 *
 * It is called by the two shells at both ends of the connection, with
 * the arguments juggled around appropriately.
 *
 * srcNode is the remote node.
 * proxy is the Id of the object on the remote node
 * asyncFuncId identifies the function(s) to send data back to the remote
 * 	node. It is zero if there are no functions, ie, if the local dest is
 * 	unidirectional and does not need to send data back. 
 * 	It otherwise is the asyncFuncId that points to the
 * 	dest::asyncFunc which handles the send function of the dest 
 * 	and stuffs arguments into the sendBuffer of the postmaster.
 * dest is the Id of the object on the local node
 * destMsg looks up the Finfo for the message on the local object.
 *
 * This rather nasty function does a bit both of SrcFinfo::add and
 * DestFinfo::respondToAdd, since it has to bypass much of the logic of
 * both.
 */
bool setupProxyMsg( unsigned int srcNode, 
	Id proxy, unsigned int asyncFuncId, 
	Id dest, int destMsg )
{
	const Finfo* destFinfo = dest()->findFinfo( destMsg );
	assert( destFinfo != 0 );
	unsigned int pfid = destFinfo->proxyFuncId();
	assert( pfid > 0 );
	
	// Check for existence of proxy, create if needed.
	Element* pe;
#ifdef DO_UNIT_TESTS
// Some nasty stuff here because we want to test parallel message 
// setup without going off-node.
// To do this we make proxies for elements that are actualy perfectly
// legal elements residing locally.
	if ( srcNode == 0 && dest.node() == 0 ) {
		if ( proxy.good() && 
			dynamic_cast< ProxyElement* >( proxy.eref().e ) == 0 ) {
			proxy = Id::scratchId();
		}
	}
#endif
	// Shell proxy hack section. This will all go away when we have
	// proper cross-node array elements.
	if ( proxy == Id::shellId() ) {
		get< Id >( Id::postId( 0 ).eref(), "shellProxy", proxy );
		proxy = Id::scratchId();
		pe = new ProxyElement( proxy, srcNode, pfid );
		set< Id >( Id::postId( srcNode ).eref(), "shellProxy", proxy );
		assert( proxy.eref().data() == Id::postId( srcNode ).eref().data() );
		/*
		if ( proxy == Id() ) { // Not yet defined
			proxy = Id::scratchId();
			pe = new ProxyElement( proxy, srcNode, pfid );
			unsigned int numNodes = MPI::COMM_WORLD.Get_size();
			for ( unsigned int i = 0; i < numNodes; i++ )
				set< Id >( Id::postId( i ).eref(), "shellProxy", proxy );
		}
		*/
	}

	// Regular setup for proxies.
	if ( proxy.isProxy() ) {
		pe = proxy();
	} else {
		pe = new ProxyElement( proxy, srcNode, pfid );
	}
	unsigned int srcIndex = pe->numTargets( 0, proxy.index() );
	unsigned int destIndex = dest()->numTargets( destMsg, dest.index() );
	unsigned int destFuncId = destFinfo->funcId();

	bool ret = Msg::add( 
		proxy.eref(), dest.eref(), 
		0,	// This is the msg# for the srcFinfo. But the src here is a
			// proxy, which manages a single msg, so the msg# is always 0.
		destMsg,
		srcIndex, destIndex,
		asyncFuncId,// This func handles data to go back to the remote
					// node. This funcId is used to set up the
					// Msg of the local element. 
					// It is zero if no data has to go back. In other
					// words, the local (dest) element is a pure dest.
					// If it is nonzero, the function is the
					// asyncFunc, and is used by the local 'dest' element
					// to put args into the sendBuf of the postmaster.

		destFuncId,	// This is the regular RecvFunc of the dest element,
					// called by the Msg of the proxy. Note that this
					// call in turn happens when data arrives at the
					// recvBuf of the PostMaster, and the PostMaster
					// calls the proxyFunc for this proxy. The proxyFunc
					// converts the buffered data into appropriate types,
					// and then calls 'send' on the msg managed by the
					// proxy.
		ConnTainer::Default /// \todo: need to get better info on option.
	);
	
	// Need to find the type of the dest and use the add from the proxy 
	// to the dest. 
	// Uses the Ftype::proxyFuncId()
	// Proxy src type is always derived from destMsg.
	return ret;
}

/////////////////////////////////////////////////////////////////////
// This function does the main work of sending incoming messages
// to dests.
/////////////////////////////////////////////////////////////////////

/**
 * This function posts a non-blocking receive for data
 */
void PostMaster::innerPostIrecv()
{
	// cout << "!" << flush;
	cout << "inner PostIrecv on node " << localNode_ << " from " << remoteNode_ << endl << flush;
	if ( localNode_ != remoteNode_ ) {
		request_ = comm_->Irecv(
			&( recvBuf_[0] ), recvBuf_.size(), MPI_CHAR, 
				remoteNode_, DATA_TAG );
		// cout << inBufSize_ << " innerPostIrecv: request_ empty?" << ( request_ == static_cast< MPI::Request >( 0 ) ) << "\n";
		donePoll_ = 0;
	} else {
		donePoll_ = 1;
	}
}

void PostMaster::postIrecv( const Conn* c, int ordinal )
{
	static_cast< PostMaster* >( c->data() )->innerPostIrecv();
}


/**
 * Transmits data to proxy element, which then sends data out to targets.
 * Returns size of data transmitted.
 */
unsigned int proxy2tgt( const AsyncStruct& as, char* data )
{
	// Note that 'target' is actually the proxy here.
	assert( as.proxy().good() );

	Eref proxy = as.proxy().eref();
	ProxyElement* pe = dynamic_cast< ProxyElement* >( proxy.e );
	assert( pe != 0 );

	pe->sendData( as.funcIndex(), data );
	/*
	// Second arg here should be target message. For the proxy
	// this is the one and only message, so it should be zero.
	// In principle the AsyncStruct handles additional info for
	// the target message, but I am dubious about it.
	Slot s( 0, 0 ); // First msg, first func in it. Seems OK.
	send1< char* >( tgt, s, data );
	*/

	// OK, how big is the data chunk? Now this would be useful
	// to pass in the Async struct. The dest serializer knows, but
	// we can't get at the info here. Given that we may transfer
	// strings and vecs, we can't get at this info from the target
	// message either.
	// OK, put it in the Async struct. For now hacked into the
	// sourceMsg() field.
	
	return as.size();
}

/**
 * This function does the main work of sending incoming messages
 * to dests. It grinds through the irecv'ed data and sends it out
 * to local dest objects.
 * This is called by the poll function
 */
void PostMaster::innerPoll( const Conn* c )
{
	Eref e = c->target();
	cout << "inner Poll on node " << localNode_ << " from " << remoteNode_ << endl << flush;
	if ( donePoll_ )
			return;
	if ( !request_ ) {
		sendBack1< unsigned int >( c, pollSlot, remoteNode_ );
		donePoll_ = 1;
		return;
	}
	if ( request_.Test( status_ ) ) {
		// Data has arrived. How big was it?
		unsigned int dataSize = status_.Get_count( MPI_CHAR );
		cout << dataSize << " bytes of data arrived on " << localNode_ << " from " << remoteNode_;
		request_ = 0;
		if ( dataSize < sizeof( unsigned int ) ) return;

		if ( syncInfo_.size() > 0 ) {
			// Here we deal with SyncFinfos
		}

		// Handle async data in the buffer
		char* data = &( recvBuf_[ 0 ] );
		unsigned int nMsgs = *static_cast< const unsigned int* >(
						static_cast< const void* >( data ) );
		data += sizeof( unsigned int );

		AsyncStruct foo( data );
		cout << ", nMsgs = " << nMsgs << ", proxyId = " << foo.proxy() <<
			", dataSize = " << foo.size() << ", func = " << foo.funcIndex() << endl;

		for ( unsigned int i = 0; i < nMsgs; i++ ) {
			AsyncStruct as( data );

			///\todo: temporary hack to deal with shell-shell messaging
			if ( as.proxy() == Id::shellId() )
				as.hackProxy( shellProxy_ );

			data += sizeof( AsyncStruct );
			unsigned int size = proxy2tgt( as, data );
			// assert( size == as.size() );
			data += size;
		}

		donePoll_ = 1;
	}
	if ( syncInfo_.size() == 0 ) { // Do only one pass if it is async.
		sendBack1< unsigned int >( c, pollSlot, remoteNode_ );
		donePoll_ = 1;
	}
}

void PostMaster::innerBarrier( )
{
	// Just for paranoia: Only allow this function to be called for
	// postmaster 0.
	cout << "Barrier on node " << localNode_ << " from " << remoteNode_ << endl << flush;
	if ( remoteNode_ == 0 )
		MPI::COMM_WORLD.Barrier();
}

void PostMaster::poll( const Conn* c, int ordinal )
{
	static_cast< PostMaster* >( c->data() )->innerPoll( c );
}

void PostMaster::barrier( const Conn* c )
{
	static_cast< PostMaster* >( c->data() )->innerBarrier( );
}

/**
 * This uses a blocking send to send out the data. Normally this
 * should be immediate provided the iRecv has been posted at the
 * destination.
 */
void PostMaster::innerPostSend( )
{
	// send out the filled buffer here to the other nodes..
	char* data = &( sendBuf_[ 0 ] );
	*static_cast< unsigned int* >( static_cast< void* >( data ) ) = 
		numSendBufMsgs_;
	cout << "sending " << sendBufPos_ << " bytes: " << &sendBuf_[0] <<
		" from node " << localNode_ << " to " << remoteNode_;
	if ( localNode_ != remoteNode_ ) {
		comm_->Send( &( sendBuf_[0] ), sendBufPos_, 
			MPI_CHAR, remoteNode_, DATA_TAG
		);

		unsigned int nMsgs = *static_cast< const unsigned int* >(
						static_cast< const void* >( data ) );
		data += sizeof( unsigned int );
		AsyncStruct foo( data );
		cout << ", nMsgs = " << nMsgs << ", proxyId = " << foo.proxy() <<
			", dataSize = " << foo.size() << ", func = " << foo.funcIndex() << endl;

	}
	sendBufPos_ = sizeof( unsigned int );
	numSendBufMsgs_ = 0;
}

void PostMaster::postSend( const Conn* c, int ordinal )
{
	static_cast< PostMaster* >( c->data() )->innerPostSend();
}

/////////////////////////////////////////////////////////////////////

#if 0
/**
 * This static function handles requests to set up a message. It does the
 * following:
 * - posts irecv for return status of the message
 * - Sends out the message request itself
 * - Creates the local message stuff
 * - Returns index for forming local message.
 */

unsigned int PostMaster::respondToAdd(
		Element* e, const string& respondString, unsigned int numDest )
{
		/*
		cout << "\nresponding to add from node " <<
			PostMaster::getMyNode( e ) <<
		 	" to node " << 
			PostMaster::getRemoteNode( e ) <<
			" with " << respondString << endl;
			*/
	// A little risky: We assume that the second message on the
	// dataSlot of the postmaster is the connection to the Shell,
	// and that nothing is inserted below this.
	unsigned int shellIndex = 0;
	PostMaster* p = static_cast< PostMaster* >( e->data() );
	char* buf = static_cast< char* >(
		p->innerGetAsyncParBuf( shellIndex, respondString.length() + 1 )
	);
	strcpy( buf, respondString.c_str() );

	return dataSlot;
}

/**
 * This function handles addition of message from postmaster to
 * targets. Usually called from within the PostMaster following a
 * cross-node message
 */
void PostMaster::placeIncomingFuncs( 
		vector< IncomingFunc >& inFl, unsigned int msgIndex )
{
	;
}
/////////////////////////////////////////////////////////////////////

#endif
void* getParBuf( const Conn* c, unsigned int size )
{
	PostMaster* pm = static_cast< PostMaster* >( c->data() );
	assert( pm != 0 );
//	return pm->innerGetParBuf( c.targetIndex(), size );
	return 0;
}

void* getAsyncParBuf( const Conn* c, unsigned int size )
{
	PostMaster* pm = static_cast< PostMaster* >( c->data() );
	assert( pm != 0 );
	return pm->innerGetAsyncParBuf( c, size );
}

/**
 * This function puts in information about message source and target
 * function into the data buffer, followed by the arguments.
 * It returns the next free location to the calling function.
 * It internally increments the current location of the buffer.
 * If we don't use MPI, then this whole file is unlikely to be compiled.
 * So we define the dummy version of the function in DerivedFtype.cpp.
 */
void* PostMaster::innerGetAsyncParBuf( const Conn* c, unsigned int size )
{
	if ( size + sendBufPos_ > sendBuf_.size() ) {
		cout << "in getParBuf: Out of space in sendBuf_\n";
		// Do something clever here to send another installment
		return 0;
	}
	// We pass the source id here because it is used to look up
	// the proxy on the target node.
	// I'm not sure the targetMsg is meaningful here. It may just
	// get set to whatever the postmaster AsyncMsgDest uses.
	// I'm pretty sure the sourceMsg is useless too.
	// I'm going to put the message size in the third arg.
	AsyncStruct as( c->source().id(), c->funcIndex(), size );
	char* sendBufPtr = &( sendBuf_[ sendBufPos_ ] );
	*static_cast< AsyncStruct* >( static_cast< void* >( sendBufPtr ) ) = as;
	sendBufPtr += sizeof( AsyncStruct );
	sendBufPos_ += sizeof( AsyncStruct ) + size;
	++numSendBufMsgs_;
	return static_cast< void* >( sendBufPtr );
}

/////////////////////////////////////////////////////////////////////

#ifdef DO_UNIT_TESTS
#include "../element/Neutral.h"
#include "../element/Wildcard.h"
#include "Ftype2.h"
#include "setget.h"
#include "../builtins/Interpol.h"
#include "../builtins/Table.h"
#include "../shell/Shell.h"

extern void testMess( Element* e, unsigned int numNodes );
extern void pollPostmaster(); // Defined in maindir/mpiSetup.cpp

static Slot iSlot;
static Slot xSlot;
static Slot sSlot;
static Slot idVecSlot;
static Slot sVecSlot;
static Slot iSharedSlot;
static Slot xSharedSlot;
static Slot logSrcSlot;
static Slot logReturnSlot;

class TestParClass {
	public:
		static void sendI( const Conn* c ) {
			TestParClass* tp = static_cast< TestParClass* >( c->data() );
			send1< int >( c->target(), iSlot, tp->i_ ) ;
		}
		static void sendX( const Conn* c ) {
			TestParClass* tp = static_cast< TestParClass* >( c->data() );
			send1< double >( c->target(), xSlot, tp->x_ ) ;
		}
		static void sendS( const Conn* c ) {
			TestParClass* tp = static_cast< TestParClass* >( c->data() );
			send1< string >( c->target(), sSlot, tp->s_ ) ;
		}
		static void sendIdVec( const Conn* c ) {
			TestParClass* tp = static_cast< TestParClass* >( c->data() );
			send1< vector< Id > >( c->target(), idVecSlot, tp->idVec_ ) ;
		}
		static void sendSvec( const Conn* c ) {
			TestParClass* tp = static_cast< TestParClass* >( c->data() );
			send1< vector< string > >( c->target(), sVecSlot, tp->sVec_ ) ;
		}

		static void sendIshared( const Conn* c ) {
			TestParClass* tp = static_cast< TestParClass* >( c->data() );
			send1< int >( c->target(), iSharedSlot, tp->i_ ) ;
		}
		static void sendXshared( const Conn* c ) {
			TestParClass* tp = static_cast< TestParClass* >( c->data() );
			send1< double >( c->target(), xSharedSlot, tp->x_ ) ;
		}

		static void sendLog( const Conn* c ) {
			TestParClass* tp = static_cast< TestParClass* >( c->data() );
			send1< int >( c->target(), logSrcSlot, tp->i_ ) ;
		}

		static void logFunc( const Conn* c, int value )
		{
			TestParClass* tp = static_cast< TestParClass* >( c->data() );
			tp->i_ = 2 * value;
			tp->x_ = log( static_cast< double >( value ) );
			sendBack1< double >( c, logReturnSlot, exp( tp->x_ * 2.0 ) );
		}

		static void setI( const Conn* c, int value )
		{
			static_cast< TestParClass* >( c->data() )->i_ = value;
		}

		static void setX( const Conn* c, double value )
		{
			static_cast< TestParClass* >( c->data() )->x_ = value;
		}

		static void setS( const Conn* c, string value )
		{
			static_cast< TestParClass* >( c->data() )->s_ = value;
		}

		static void setIdVec( const Conn* c, vector< Id > value )
		{
			static_cast< TestParClass* >( c->data() )->idVec_ = value;
		}

		static void setSvec( const Conn* c, vector< string > value )
		{
			static_cast< TestParClass* >( c->data() )->sVec_ = value;
		}

		int i_;
		double x_;
		string s_;
		vector< Id > idVec_;
		vector< string > sVec_;
};

/**
 * This test class has messages for ints, doubles, strings, vectors.
 */
const Cinfo* initTestParClass()
{
	static Finfo* iFinfo = new DestFinfo( "i", Ftype1< int >::global(),
			RFCAST( &TestParClass::setI ) );
	static Finfo* xFinfo = new DestFinfo( "x", Ftype1< double >::global(),
			RFCAST( &TestParClass::setX ) );
	static Finfo* ixShared[] = 
	{
		iFinfo,
		xFinfo,
	};

	static Finfo* iSrcFinfo = 
		new SrcFinfo( "iSrc", Ftype1< int >::global() );
	static Finfo* xSrcFinfo = 
		new SrcFinfo( "xSrc", Ftype1< double >::global() );
	static Finfo* ixSrcShared[] = 
	{
		iSrcFinfo,
		xSrcFinfo,
	};

	/**
	 * This test operation takes the natural log of the incoming integer
	 * and puts it in x. Then it sends out antilog of 2x to the 
	 * the originating element, to put into the x field there.
	 */
	static Finfo* logFinfo = new DestFinfo( "log", Ftype1< int >::global(),
			RFCAST( &TestParClass::logFunc ) );
	static Finfo* logSrc[] = 
	{
		iSrcFinfo,
		xFinfo,
	};

	static Finfo* logDest[] = 
	{
		logFinfo,
		xSrcFinfo,
	};

	static Finfo* testParClassFinfos[] =
	{
		iFinfo,
		xFinfo,
		new DestFinfo( "s", Ftype1< string >::global(),
			RFCAST( &TestParClass::setS ) ),
		new DestFinfo( "idVec", Ftype1< vector< Id > >::global(),
			RFCAST( &TestParClass::setIdVec ) ),
		new DestFinfo( "sVec", Ftype1< vector< string > >::global(),
			RFCAST( &TestParClass::setSvec ) ),

		iSrcFinfo,
		xSrcFinfo,
		new SrcFinfo( "sSrc", Ftype1< string >::global() ),
		new SrcFinfo( "idVecSrc", Ftype1< vector< Id > >::global() ),
		new SrcFinfo( "sVecSrc", Ftype1< vector< string > >::global() ),

		new SharedFinfo( "ix", ixShared,
			sizeof( ixShared ) / sizeof( Finfo* ) ),
		new SharedFinfo( "ixSrc", ixSrcShared,
			sizeof( ixSrcShared ) / sizeof( Finfo* ) ),
		new SharedFinfo( "logSrc", logSrc,
			sizeof( logSrc ) / sizeof( Finfo* ) ),
		new SharedFinfo( "logDest", logDest,
			sizeof( logDest ) / sizeof( Finfo* ) ),
	};

	static Cinfo testParClassCinfo(
		"TestPar",
		"Upi Bhalla, 2008, NCBS",
		"Test class for parallel messaging",
		initNeutralCinfo(),
		testParClassFinfos,
		sizeof( testParClassFinfos ) / sizeof( Finfo* ),
		ValueFtype1< TestParClass >::global()
	);
	return & testParClassCinfo;
}

void testParAsyncObj2Post()
{
	cout << "\nTesting Par Async: Obj2Post";
	const Cinfo* tpCinfo = initTestParClass();
	iSlot = tpCinfo->getSlot( "iSrc" );
	xSlot = tpCinfo->getSlot( "xSrc" );
	sSlot = tpCinfo->getSlot( "sSrc" );
	idVecSlot = tpCinfo->getSlot( "idVecSrc" );
	sVecSlot = tpCinfo->getSlot( "sVecSrc" );

	FuncVec::sortFuncVec();

	Element* n = Neutral::create( "Neutral", "n", Id(), Id::scratchId() );

	Element* p = Neutral::create(
			"PostMaster", "node0", n->id(), Id::scratchId());
	ASSERT( p != 0, "created Test Postmaster" );

	Element* t = 
		Neutral::create( "TestPar", "tp", n->id(), Id::scratchId() );
	ASSERT( t != 0, "created TestPar" );
	TestParClass* tdata = static_cast< TestParClass* >( t->data() );

	SetConn c( t, 0 );

	PostMaster* pm = static_cast< PostMaster* >( p->data() );

	// Send an int to the postmaster
	tdata->i_ = 44332211;
	bool ret = Eref( t ).add( "iSrc", p, "async" );
	ASSERT( ret, "msg to post" );
	TestParClass::sendI( &c );
	char* abuf = &( pm->sendBuf_[ sizeof( unsigned int ) ] );
	AsyncStruct as( abuf );
	// int asyncMsgNum = p->findFinfo( "async" )->msg();
	// int iSendMsgNum = t->findFinfo( "iSrc" )->msg();
	ASSERT( as.proxy() == t->id(), "async info to post" );
	ASSERT( as.funcIndex() == 0, "async info to post" );
	// ASSERT( as.sourceMsg() == iSendMsgNum, "async info to post" );
	ASSERT( as.size() == sizeof( int ), "async info to post" );

	void* vabuf = static_cast< void* >( abuf + sizeof( AsyncStruct ) );
	int i = *static_cast< int* >( vabuf );
	ASSERT( i == tdata->i_, "sending int" );
	i = 0;
	Serializer< int >::unserialize( i, vabuf ); // The postmaster uses unserialize
	ASSERT( i == tdata->i_, "sending int" );

	// Send a double to the postmaster
	pm->sendBufPos_ = sizeof( unsigned int );
	tdata->x_ = 3.1415926535;
	ret = Eref( t ).add( "xSrc", p, "async" );
	ASSERT( ret, "msg to post" );
	TestParClass::sendX( &c );
	double x = *static_cast< double* >( vabuf );
	ASSERT( x == tdata->x_, "sending double" );
	x = 0;
	Serializer< double >::unserialize( x, vabuf ); // The postmaster uses unserialize
	ASSERT( x == tdata->x_, "sending double" );

	// Send a string to the postmaster
	pm->sendBufPos_ = sizeof( unsigned int );
	tdata->s_ = "Gleams that untravelled world";
	ret = Eref( t ).add( "sSrc", p, "async" );
	ASSERT( ret, "msg to post" );
	TestParClass::sendS( &c );
	string s;
	Serializer< string >::unserialize( s, vabuf );
	// string s = *static_cast< string* >( vabuf );
	ASSERT( s == tdata->s_, "sending string" );

	// Send a vector of Ids to the postmaster
	pm->sendBufPos_ = sizeof( unsigned int );
	unsigned int nfoo = simpleWildcardFind( "/##", tdata->idVec_ );
	ret = Eref( t ).add( "idVecSrc", p, "async" );
	ASSERT( ret, "msg to post" );
	TestParClass::sendIdVec( &c );
	vector< Id > foo;
	// foo = *static_cast< vector< Id >* >( vabuf );
	Serializer< vector< Id > >::unserialize( foo, vabuf );
	ASSERT( nfoo == foo.size(), "sending vector< Id >" );
	ASSERT( foo == tdata->idVec_, "sending vector< Id >" );

	// Send a vector of strings to the postmaster
	pm->sendBufPos_ = sizeof( unsigned int );
	tdata->sVec_.push_back( "whose margin fades" );
	tdata->sVec_.push_back( "forever and forever" );
	tdata->sVec_.push_back( "when I move" );
	vector< string > svec = tdata->sVec_;

	ret = Eref( t ).add( "sVecSrc", p, "async" );
	ASSERT( ret, "msg to post" );
	TestParClass::sendSvec( &c );
	vector< string > bar;
	Serializer< vector< string > >::unserialize( bar, vabuf );
	// vector< string > bar = *static_cast< vector< string >* >( vabuf );
	ASSERT( svec == bar, "sending vector< string >" );

	// Make a message to an int via a proxy
	// Make a message to 100 ints via the proxy
	// Send a value to an int through the proxy
	//
	// Repeat for a vector of strings or something equally awful.
	//

	// Using a buffer-to-buffer memcpy, do a single-node check on the
	// complete transfer from int to int and all of the above.
	//
	// Also confirm that they work with multiple messages appearing
	// in random order each time.
	set( n, "destroy" );
}

void testParAsyncObj2Post2Obj()
{
	cout << "\nTesting Parallel Async: Obj2Obj via post. ";
	const Cinfo* tpCinfo = initTestParClass();
	iSlot = tpCinfo->getSlot( "iSrc" );
	xSlot = tpCinfo->getSlot( "xSrc" );
	sSlot = tpCinfo->getSlot( "sSrc" );
	idVecSlot = tpCinfo->getSlot( "idVecSrc" );
	sVecSlot = tpCinfo->getSlot( "sVecSrc" );
	iSharedSlot = tpCinfo->getSlot( "ixSrc.iSrc" );
	xSharedSlot = tpCinfo->getSlot( "ixSrc.xSrc" );

	Element* n = Neutral::create( "Neutral", "n", Id(), Id::scratchId() );

	Element* p0 = Neutral::create(
			"PostMaster", "node0", n->id(), Id::scratchId());
	ASSERT( p0 != 0, "created Src Postmaster" );

	Element* p1 = Neutral::create(
			"PostMaster", "node1", n->id(), Id::scratchId());
	ASSERT( p1 != 0, "created Dest Postmaster" );

	Element* t = 
		Neutral::create( "TestPar", "tp", n->id(), Id::scratchId() );
	ASSERT( t != 0, "created TestPar" );
	TestParClass* tdata = static_cast< TestParClass* >( t->data() );

	Element* tdest = 
		Neutral::create( "TestPar", "tdest", n->id(), Id::scratchId() );
	TestParClass* tDestData = static_cast< TestParClass* >( tdest->data() );

	SetConn c( t, 0 );

	PostMaster* pm0 = static_cast< PostMaster* >( p0->data() );
	PostMaster* pm1 = static_cast< PostMaster* >( p1->data() );
	pm0->sendBufPos_ = sizeof( unsigned int ); // to count # of msgs
	void* vabuf = static_cast< void* >( &pm0->sendBuf_[0] );

	///////////////////////////////////////////////////////////////////
	// Set up the data to send
	///////////////////////////////////////////////////////////////////
	*static_cast< unsigned int * >( vabuf ) = 5; // set it to 5 msgs.
	tdata->i_ = 44332200;
	tDestData->i_ = 0;
	tdata->x_ = 0.1234;
	tDestData->x_ = 0.0;
	tdata->s_ = "You eat your victuals fast enough";
	tDestData->s_ = "";
	simpleWildcardFind( "/##", tdata->idVec_ );
	tDestData->idVec_.resize( 0 );
	tdata->sVec_.resize( 0 );
	tdata->sVec_.push_back( "Terence" );
	tdata->sVec_.push_back( "this is" );
	tdata->sVec_.push_back( "stupid stuff" );
	tDestData->sVec_.resize( 0 );

	///////////////////////////////////////////////////////////////////
	// Set up the messages
	///////////////////////////////////////////////////////////////////
	bool ret = Eref( t ).add( "iSrc", p0, "async" );
	ASSERT( ret, "msg to post" );
	int tgtMsg = tdest->findFinfo( "i" )->msg();
	Id proxyId[ 6 ];

	// proxyId[ 0 ] = Id::scratchId();
	// ret = setupProxyMsg( 0, proxyId[ 0 ], 0, tdest->id(), tgtMsg );
	ret = setupProxyMsg( 0, t->id(), 0, tdest->id(), tgtMsg );
	// The setupProxyMsg creates a new proxy specially for these tests.
	proxyId[0] = Id::lastId(); 

	ASSERT( ret, "msg from post to tgt i" );
	///////////////////////////////////////////

	ret = Eref( t ).add( "xSrc", p0, "async" );
	ASSERT( ret, "msg to post" );
	tgtMsg = tdest->findFinfo( "x" )->msg();

	ret = setupProxyMsg( 0, t->id(), 0, tdest->id(), tgtMsg );
	proxyId[ 1 ] = Id::lastId();
	ASSERT( ret, "msg from post to tgt x" );
	///////////////////////////////////////////

	ret = Eref( t ).add( "sSrc", p0, "async" );
	ASSERT( ret, "msg to post" );
	tgtMsg = tdest->findFinfo( "s" )->msg();

	ret = setupProxyMsg( 0, t->id(), 0, tdest->id(), tgtMsg );
	proxyId[ 2 ] = Id::lastId();
	ASSERT( ret, "msg from post to tgt s" );
	///////////////////////////////////////////

	ret = Eref( t ).add( "idVecSrc", p0, "async" );
	ASSERT( ret, "msg to post" );
	tgtMsg = tdest->findFinfo( "idVec" )->msg();

	ret = setupProxyMsg( 0, t->id(), 0, tdest->id(), tgtMsg );
	proxyId[ 3 ] = Id::lastId();
	ASSERT( ret, "msg from post to tgt idVec" );
	///////////////////////////////////////////

	ret = Eref( t ).add( "sVecSrc", p0, "async" );
	ASSERT( ret, "msg to post" );
	tgtMsg = tdest->findFinfo( "sVec" )->msg();

	ret = setupProxyMsg( 0, t->id(), 0, tdest->id(), tgtMsg );
	proxyId[ 4 ] = Id::lastId();
	ASSERT( ret, "msg from post to tgt sVec" );
	///////////////////////////////////////////

	ret = Eref( t ).add( "ixSrc", p0, "async" );
	ASSERT( ret, "Shared msg to post" );
	tgtMsg = tdest->findFinfo( "ix" )->msg();

	ret = setupProxyMsg( 0, t->id(), 0, tdest->id(), tgtMsg );
	proxyId[ 5 ] = Id::lastId();
	ASSERT( ret, "msg from post to tgt ix" );

	///////////////////////////////////////////////////////////////////
	// Send the data to pm0
	///////////////////////////////////////////////////////////////////
	TestParClass::sendI( &c );
	char* abuf = &( pm0->sendBuf_[0] ) + sizeof( unsigned int );
	AsyncStruct as( abuf );

	vabuf = static_cast< void* >( abuf + sizeof( AsyncStruct ) );
	int i = *static_cast< int* >( vabuf );
	ASSERT( i == tdata->i_, "sending int" );
	i = 0;
	Serializer< int >::unserialize( i, vabuf ); // The postmaster uses unserialize
	ASSERT( i == tdata->i_, "sending int" );

	TestParClass::sendX( &c );
	TestParClass::sendS( &c );
	TestParClass::sendIdVec( &c );
	TestParClass::sendSvec( &c );

	///////////////////////////////////////////////////////////
	// Here I copy over the contents of the send buf of pm0 to 
	// the recv buf of pm1, to simulate the process of MPI-based
	// internode data transmission.
	///////////////////////////////////////////////////////////
	pm1->recvBuf_ = pm0->sendBuf_;

	// SetConn p1c( p1, 0 );
	//PostMaster::innerPoll( p1c ); // This deals with the incoming data

	// Some lines pinched from innerPoll
	// Handle async data in the buffer
	char* data = &( pm1->recvBuf_[ 0 ] );
	unsigned int nMsgs = *static_cast< const unsigned int* >(
					static_cast< const void* >( data ) );
	ASSERT( nMsgs == 5, "on tgt postmaster");
	data += sizeof( unsigned int );

	ASSERT( tDestData->i_ != tdata->i_, "Pre int data on tgt" );
	ASSERT( tDestData->x_ != tdata->x_, "Pre double data on tgt" );
	ASSERT( tDestData->s_ != tdata->s_, "Pre string data on tgt" );
	ASSERT( tDestData->idVec_ != tdata->idVec_, "Pre idVec data on tgt" );
	ASSERT( tDestData->sVec_ != tdata->sVec_, "Pre sVec data on tgt" );

	for ( unsigned int i = 0; i < nMsgs; i++ ) {
		AsyncStruct as( data );
		// Here we fudge it because the AsyncStruct still has the
		// orignal element id, but we need to put in the proxy id
		// Since we are testing it all on the same node it has to
		// be a different id.
		as.proxy_ = proxyId[ i ];
		data += sizeof( AsyncStruct );
		unsigned int size = proxy2tgt( as, data );
		// assert( size == as.size() );
		data += size;
	}
	ASSERT( tDestData->i_ == tdata->i_, "Recieved int data on tgt" );
	ASSERT( tDestData->x_ == tdata->x_, "Recieved double data on tgt" );
	ASSERT( tDestData->s_ == tdata->s_, "Recieved string data on tgt" );
	ASSERT( tDestData->idVec_ == tdata->idVec_, "Recieved idVec data on tgt" );
	ASSERT( tDestData->sVec_ == tdata->sVec_, "Recieved sVec data on tgt" );

	///////////////////////////////////////////////////////////////////
	// Send another installment, this time doing the i and x via a
	// shared message, and shuffling the order around.
	///////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////
	// First, set up the data to send
	///////////////////////////////////////////////////////////////////
	pm0->sendBufPos_ = sizeof( unsigned int ); // to count # of msgs
	*static_cast< unsigned int * >( vabuf ) = 5; // set it to 5 msgs.
	tdata->i_ = 98765;
	tDestData->i_ = 0;
	tdata->x_ = 0.9876;
	tDestData->x_ = 0.0;
	tdata->s_ = "There's nothing wrong with you, tis clear";
	tDestData->s_ = "";
	tdata->idVec_.resize( 5 );
	tdata->idVec_.push_back( proxyId[3] );
	tDestData->idVec_.resize( 0 );
	tdata->sVec_.resize( 0 );
	tdata->sVec_.push_back( "To see the" );
	tdata->sVec_.push_back( "rate you" );
	tdata->sVec_.push_back( "drink your beer" );
	tDestData->sVec_.resize( 0 );
	///////////////////////////////////////////////////////////////////
	// Send it.
	///////////////////////////////////////////////////////////////////
	TestParClass::sendIdVec( &c );
	TestParClass::sendIshared( &c );
	TestParClass::sendSvec( &c );
	TestParClass::sendXshared( &c );
	TestParClass::sendS( &c );

	///////////////////////////////////////////////////////////////////
	// Use a new vec of proxies, because we're sending in a different order
	///////////////////////////////////////////////////////////////////
	vector< Id > nproxyId;
	nproxyId.push_back( proxyId[3] );
	nproxyId.push_back( proxyId[5] );
	nproxyId.push_back( proxyId[4] );
	nproxyId.push_back( proxyId[5] );
	nproxyId.push_back( proxyId[2] );

	///////////////////////////////////////////////////////////
	// Here I copy over the contents of the send buf of pm0 to 
	// the recv buf of pm1, to simulate the process of MPI-based
	// internode data transmission.
	///////////////////////////////////////////////////////////
	pm1->recvBuf_ = pm0->sendBuf_;

	// SetConn p1c( p1, 0 );
	//PostMaster::innerPoll( p1c ); // This deals with the incoming data

	// Some lines pinched from innerPoll
	// Handle async data in the buffer
	data = &( pm1->recvBuf_[ 0 ] );
	nMsgs = *static_cast< const unsigned int* >(
					static_cast< const void* >( data ) );
	ASSERT( nMsgs == 5, "on tgt postmaster");
	data += sizeof( unsigned int );

	ASSERT( tDestData->i_ != tdata->i_, "Pre int data on tgt" );
	ASSERT( tDestData->x_ != tdata->x_, "Pre double data on tgt" );
	ASSERT( tDestData->s_ != tdata->s_, "Pre string data on tgt" );
	ASSERT( tDestData->idVec_ != tdata->idVec_, "Pre idVec data on tgt" );
	ASSERT( tDestData->sVec_ != tdata->sVec_, "Pre sVec data on tgt" );

	for ( unsigned int i = 0; i < nMsgs; i++ ) {
		AsyncStruct as( data );
		// Here we fudge it because the AsyncStruct still has the
		// orignal element id, but we need to put in the proxy id
		// Since we are testing it all on the same node it has to
		// be a different id.
		as.proxy_ = nproxyId[ i ];
		data += sizeof( AsyncStruct );
		unsigned int size = proxy2tgt( as, data );
		// assert( size == as.size() );
		data += size;
	}
	ASSERT( tDestData->i_ == tdata->i_, "Received int data on tgt" );
	ASSERT( tDestData->x_ == tdata->x_, "Received double data on tgt" );
	ASSERT( tDestData->s_ == tdata->s_, "Received string data on tgt" );
	ASSERT( tDestData->idVec_ == tdata->idVec_, "Received idVec data on tgt" );
	ASSERT( tDestData->sVec_ == tdata->sVec_, "Received sVec data on tgt" );

	set( n, "destroy" );
}

/**
 * This tests shell interfacing to set up messages. It could just as
 * well be in the Shell tests, but I think it fits in better here
 * as it is all parallel stuff.
 */
void testShellSetupAsyncParMsg()
{
	cout << "\nTesting Shell setup of Parallel Async";
	const Cinfo* tpCinfo = initTestParClass();
	iSlot = tpCinfo->getSlot( "iSrc" );
	xSlot = tpCinfo->getSlot( "xSrc" );
	sSlot = tpCinfo->getSlot( "sSrc" );
	idVecSlot = tpCinfo->getSlot( "idVecSrc" );
	sVecSlot = tpCinfo->getSlot( "sVecSrc" );
	iSharedSlot = tpCinfo->getSlot( "ixSrc.iSrc" );
	xSharedSlot = tpCinfo->getSlot( "ixSrc.xSrc" );

	Element* n = Neutral::create( "Neutral", "n", Id(), Id::scratchId() );

	Element* p0 = Neutral::create(
			"PostMaster", "node0", n->id(), Id::scratchId());
	ASSERT( p0 != 0, "created Src Postmaster" );

	Element* p1 = Neutral::create(
			"PostMaster", "node1", n->id(), Id::scratchId());
	ASSERT( p1 != 0, "created Dest Postmaster" );

	Element* t = 
		Neutral::create( "TestPar", "tp", n->id(), Id::scratchId() );
	ASSERT( t != 0, "created TestPar" );
	TestParClass* tdata = static_cast< TestParClass* >( t->data() );

	Element* tdest = 
		Neutral::create( "TestPar", "tdest", n->id(), Id::scratchId() );
	TestParClass* tDestData = static_cast< TestParClass* >( tdest->data() );

	PostMaster* pm0 = static_cast< PostMaster* >( p0->data() );
	PostMaster* pm1 = static_cast< PostMaster* >( p1->data() );
	pm0->sendBufPos_ = sizeof( unsigned int ); // to count # of msgs
	void* vabuf = static_cast< void* >( &pm0->sendBuf_[0] );

	///////////////////////////////////////////////////////////////////
	// Set up the data to send
	///////////////////////////////////////////////////////////////////
	*static_cast< unsigned int * >( vabuf ) = 1; // set it to 1 msgs.
	tdata->i_ = 44332200;
	tDestData->i_ = 0;
	tdata->x_ = 0.1234;
	tDestData->x_ = 0.0;
	tdata->s_ = "And malt does more than Milton can";
	tDestData->s_ = "";
	simpleWildcardFind( "/##", tdata->idVec_ );
	tDestData->idVec_.resize( 0 );
	tdata->sVec_.resize( 0 );
	tdata->sVec_.push_back( "To justify" );
	tdata->sVec_.push_back( "God's ways" );
	tdata->sVec_.push_back( "to man" );
	tDestData->sVec_.resize( 0 );

	///////////////////////////////////////////////////////////////////
	// Set up the the shells
	///////////////////////////////////////////////////////////////////
	
	Element* esh0 = Neutral::create( "Shell", "sh0", n->id(), Id::scratchId() );
	Element* esh1 = Neutral::create( "Shell", "sh1", n->id(), Id::scratchId() );
	Shell *sh0 = static_cast< Shell* >( esh0->data( 0 ) );
	Shell *sh1 = static_cast< Shell* >( esh1->data( 0 ) );
	sh0->post_ = p0;
	sh1->post_ = p1;

	bool ret = Eref( esh0 ).add( "parallel", esh1, "parallel" );
	ASSERT( ret, "Setting up msg between shells" );
	SetConn c( esh0 , 0 );
	Shell::addParallelSrc( &c, t->id(), "sVecSrc", tdest->id(), "sVec" );

	/////////////////////////////////////////////////////////////////
	// Activate the message.
	/////////////////////////////////////////////////////////////////
	SetConn ct( t , 0 );
	TestParClass::sendSvec( &ct );
	pm1->recvBuf_ = pm0->sendBuf_;
	char* data = &( pm1->recvBuf_[ 0 ] );
	unsigned int nMsgs = *static_cast< const unsigned int* >(
					static_cast< const void* >( data ) );
	ASSERT( nMsgs == 1, "on tgt postmaster");
	ASSERT( tDestData->sVec_ != tdata->sVec_, "Pre sVec data on tgt" );

	// Stuff here to transfer the data.
		data += sizeof( unsigned int );
		AsyncStruct as( data );
		// Here we fudge it because the AsyncStruct still has the
		// orignal element id, but we need to put in the proxy id
		// Since we are testing it all on the same node it has to
		// be a different id.
		as.proxy_ = Id::lastId();
		data += sizeof( AsyncStruct );
		unsigned int size = proxy2tgt( as, data );
	
	ASSERT( tDestData->sVec_ == tdata->sVec_, "Recieved sVec data on tgt" );
	size = 0;

	set( n, "destroy" );
}

/**
 * This tests bidirectional async shared message passing, and is set up
 * using shell-level commands.
 */
void testBidirectionalParMsg()
{
	cout << "\nTesting bidrectional Parallel Async msgs";
	const Cinfo* tpCinfo = initTestParClass();
	logSrcSlot = tpCinfo->getSlot( "logSrc.iSrc" );
	logReturnSlot = tpCinfo->getSlot( "logDest.xSrc" );

	Element* n = Neutral::create( "Neutral", "n", Id(), Id::scratchId() );

	Element* p0 = Neutral::create(
			"PostMaster", "node0", n->id(), Id::scratchId());
	ASSERT( p0 != 0, "created Src Postmaster" );

	Element* p1 = Neutral::create(
			"PostMaster", "node1", n->id(), Id::scratchId());
	ASSERT( p1 != 0, "created Dest Postmaster" );

	Element* t = 
		Neutral::create( "TestPar", "tp", n->id(), Id::scratchId() );
	ASSERT( t != 0, "created TestPar" );
	TestParClass* tdata = static_cast< TestParClass* >( t->data() );

	Element* tdest = 
		Neutral::create( "TestPar", "tdest", n->id(), Id::scratchId() );
	TestParClass* tDestData = static_cast< TestParClass* >( tdest->data() );

	PostMaster* pm0 = static_cast< PostMaster* >( p0->data() );
	PostMaster* pm1 = static_cast< PostMaster* >( p1->data() );
	PostMaster* post0 = 
		static_cast< PostMaster* >( Id::postId( 0 ).eref().data() );
	post0->sendBufPos_ = sizeof( unsigned int );
	pm0->sendBufPos_ = sizeof( unsigned int ); // to count # of msgs
	void* vabuf = static_cast< void* >( &post0->sendBuf_[0] );

	///////////////////////////////////////////////////////////////////
	// Set up the data to send
	///////////////////////////////////////////////////////////////////
	*static_cast< unsigned int * >( vabuf ) = 1; // set it to 1 msgs.
	tdata->i_ = 16;
	tDestData->i_ = 0;
	tdata->x_ = 0.0;
	tDestData->x_ = 0.0;

	///////////////////////////////////////////////////////////////////
	// Set up the the shells
	///////////////////////////////////////////////////////////////////
	
	Element* esh0 = Neutral::create( "Shell", "sh0", n->id(), Id::scratchId() );
	Element* esh1 = Neutral::create( "Shell", "sh1", n->id(), Id::scratchId() );
	Shell *sh0 = static_cast< Shell* >( esh0->data( 0 ) );
	Shell *sh1 = static_cast< Shell* >( esh1->data( 0 ) );
	sh0->post_ = p0;
	sh1->post_ = p1;

	bool ret = Eref( esh0 ).add( "parallel", esh1, "parallel" );
	ASSERT( ret, "Setting up msg between shells" );
	SetConn c( esh0 , 0 );
	Shell::addParallelSrc( &c, t->id(), "logSrc", tdest->id(), "logDest" );
	Id proxy1 = Id::lastId(); 
	// Proxy0 was created one before proxy1.
	Id proxy0( proxy1.id() - 1, proxy1.index() );

	/////////////////////////////////////////////////////////////////
	// Activate the message. This part is similar to what we did
	// earlier, we are just sending data forward. Main changes here
	// are that we send it via an intermediate proxy.
	/////////////////////////////////////////////////////////////////
	SetConn ct( t , 0 );
	// This does the following operations:
	//		tp->i_ = 2 * value;
	//		tp->x_ = log( static_cast< double >( value ) );
	//		sendBack1< double >( c, logReturnSlot, exp( tp->x_ * 2.0 ) );
	TestParClass::sendLog( &ct );
	// pm1->recvBuf_ = pm0->sendBuf_;
	// When data is sent into a proxyElement, it now dumps it into
	// the appropriate system postmaster.
	pm1->recvBuf_ = post0->sendBuf_;
	char* data = &( pm1->recvBuf_[ 0 ] );
	// Here we need to reset the buffer because more stuff
	// is due to come the other way as soon as we evaluate the msg.
	post0->sendBufPos_ = sizeof( unsigned int );
	
	unsigned int nMsgs = *static_cast< const unsigned int* >(
					static_cast< const void* >( data ) );
	ASSERT( nMsgs == 1, "on tgt postmaster");

	// Stuff here to transfer the data.
		data += sizeof( unsigned int );
		AsyncStruct as( data );
		// Here we fudge it because the AsyncStruct still has the
		// orignal element id, but we need to put in the proxy id
		// Since we are testing it all on the same node it has to
		// be a different id. This Id is simply the most recently created
		// object.
		as.proxy_ = proxy1;
		data += sizeof( AsyncStruct );
		unsigned int size = proxy2tgt( as, data );
	
	ASSERT( tDestData->i_ == 2 * tdata->i_, "Recieved log data on tgt" );
	ASSERT( tDestData->x_ == log( static_cast < double >( tdata->i_ ) ),
		"Recieved log data on tgt" );
	size = 0;

	/////////////////////////////////////////////////////////////////
	// Return data along the message. This is the new part.
	/////////////////////////////////////////////////////////////////
	pm0->recvBuf_ = post0->sendBuf_;
	data = &( pm0->recvBuf_[ 0 ] );
	
	nMsgs = *static_cast< const unsigned int* >(
					static_cast< const void* >( data ) );
	ASSERT( nMsgs == 1, "on original postmaster");

	// Stuff here to transfer the data.
		data += sizeof( unsigned int );
		AsyncStruct as2( data );
		// Here we fudge it because the AsyncStruct still has the
		// orignal element id, but we need to put in the proxy id
		// Since we are testing it all on the same node it has to
		// be a different id. This Id is simply the most recently created
		// object.
		as2.proxy_ = proxy0;
		data += sizeof( AsyncStruct );
		size = proxy2tgt( as2, data );
	
	// Need to allow for roundoff error here.
	ASSERT( static_cast< int >( tdata->x_ + 1e-8 ) == tdata->i_ * tdata->i_,
		"Recieved return data on src" );
	size = 0;

	set( n, "destroy" );
}

/**
 * This tests the basic parallel messaging using dummy calls on a single
 * node.
 */
void testParMsgOnSingleNode()
{
	testParAsyncObj2Post();
	testParAsyncObj2Post2Obj();
	testShellSetupAsyncParMsg();
	testBidirectionalParMsg();
}

/**
 * This is an unusual unit test: it MUST run on all nodes, otherwise
 * the system will freeze.
 * First, it tests that the automatically set up messages between shells
 * are OK.
 * Then it sets up messages between tables, going across nodes,
 * and confirms data transfer.
 */
void testPostMaster()
{
	cout << "\nTesting PostMaster across nodes";
	Element* n = Neutral::create( "Neutral", "n", Id(), Id::scratchId() );
	// First, ensure that all nodes are synced.
	MPI::COMM_WORLD.Barrier();
	unsigned int myNode = MPI::COMM_WORLD.Get_rank();
	unsigned int numNodes = MPI::COMM_WORLD.Get_size();
	unsigned int i;
	if ( myNode == 0 )
		cout << "\nTesting PostMaster: " << numNodes << " nodes";
	MPI::COMM_WORLD.Barrier();
	///////////////////////////////////////////////////////////////
	// check that we have postmasters for each of the other nodes
	// Print out a dot for each node.
	///////////////////////////////////////////////////////////////
	for ( i = 0; i < numNodes; i++ ) {
		Id id = Id::postId( i );
		ASSERT( !id.bad(), "postmasters element creation" );
		Eref p = id.eref();
		ASSERT( p.e != 0, "Check PostMaster" );
		ASSERT( p->className() == "PostMaster", "Check PostMaster");
		unsigned int remoteNode;
		get< unsigned int >( p, "remoteNode", remoteNode );
		ASSERT( remoteNode == i, "CheckPostMaster" );
	}

	unsigned int numTargets = 
		Id::shellId().eref().e->numTargets( "parallel" );
	ASSERT( ( numTargets == numNodes - 1 ),
		"Checking shell-shell messages" );
	Conn * c = Id::shellId().eref().e->targets( "parallel", 0 );
	i = 0;
	while ( c->good() ) {
		if ( i == myNode )
			i++;
		Id tgt = c->target().id();
		ASSERT( tgt.isProxy(), "Checking shell-shell msgs" );
		cout << "myNode = " << myNode << ", tgt node = " << tgt.node() << ", i = " << i << endl;
		ASSERT( tgt.eref().data() == Id::postId( i ).eref().data(), 
			"Checking proxy match to postmaster." );
		i++;
		c->increment();
	}
	delete c;

	MPI::COMM_WORLD.Barrier();
	cout << "b" << myNode;
	MPI::COMM_WORLD.Barrier();
	if ( myNode == 0 ) {
		Slot remoteCreateSlot = 
			initShellCinfo()->getSlot( "parallel.createSrc" );
		for ( i = 1; i < numNodes; i++ ) {
			char name[20];
			sprintf( name, "tn%d", i );
			string sname = name;
			unsigned int tgt = ( i < myNode ) ? i : i - 1;
			Id newId = Id::makeIdOnNode( i );
			cout << "Create op: sendTo4( shellId, slot, " << tgt << ", "
				<< "Neutral, " << sname << ", root, " << newId << endl;
			sendTo4< string, string, Id, Id >(
				Id::shellId().eref(), remoteCreateSlot, tgt,
				"Neutral", sname, 
				Id(), newId
			);
		}
	}
	MPI::COMM_WORLD.Barrier();
	pollPostmaster(); // There is a barrier in the polling operation itself
	pollPostmaster();
	MPI::COMM_WORLD.Barrier();
	char name[20];
	sprintf( name, "/tn%d", myNode );
	string sname = name;
	Id tnId( sname );
	if ( myNode != 0 )
		ASSERT( tnId.good(), "postmaster created obj on remote node" );
	MPI::COMM_WORLD.Barrier();
#ifdef TABLE_DATA
	///////////////////////////////////////////////////////////////
	// Test message transfer between tables.
	///////////////////////////////////////////////////////////////
	// On all nodes, create a table and fill it up.
	Element* table = Neutral::create( "Table", "tab", Id(), 
		Id::scratchId() );
	// cout << myNode << ": tabId = " << table->id() << endl;
	ASSERT( table != 0, "Checking data flow" );
	set< int >( table, "xdivs", 10 );
	if ( myNode == 0 ) {
		set< int >( table, "stepmode", 2 ); // TAB_ONCE
		for ( unsigned int i = 0; i <= 10; i++ )
			lookupSet< double, unsigned int >( 
							table, "table", i * i, i );
	} else {
		set< int >( table, "stepmode", 3 ); // TAB_BUF
		for ( unsigned int i = 0; i <= 10; i++ )
			lookupSet< double, unsigned int >( 
							table, "table", 0.0, i );
	}

	MPI::COMM_WORLD.Barrier();
	cout << "b" << myNode;
	MPI::COMM_WORLD.Barrier();

	if ( myNode == 0 ) {
		// Here we are being sneaky because we have the same id on all 
		// nodes.
		for ( i = 1; i < numNodes; i++ ) {
			Eref post = Id::postId( i ).eref();
			set< Id >( post, "targetId", table->id() );
			set< string >( post, "targetField", "msgInput" );
			bool ret = Eref( table ).add( "outputSrc", post, "data" );
			ASSERT( ret, "Node 0 Making input message to postmaster" );
		}
	}

	// This section sends the data over to the remote node.
	Id cjId( "/sched/cj", "/" );
	assert( !cjId.bad() );
	Element* cj = cjId();
	set< double >( cj, "start", 1.0 );

	MPI::COMM_WORLD.Barrier();
	set( table, "destroy" );
	// unsigned int cjId = Shell::path2eid( "/sched/cj", "/" );

	////////////////////////////////////////////////////////////////
	// Now we fire up the scheduler on all nodes to keep info flowing.
	////////////////////////////////////////////////////////////////
	MPI::COMM_WORLD.Barrier();
	// sleep( 5 );
	MPI::COMM_WORLD.Barrier();
	char sendstr[50];

	for ( i = 0; i < numNodes; i++ ) {
		if ( i == myNode )
			continue;
		// post = postId[i]();
		// PostMaster* pdata = static_cast< PostMaster* >( post->data() );
		sprintf( sendstr, "My name is Michael Caine %d,%d", myNode, i );

		// Find the Conn# of the message to the shell. Assume same
		// index is used on all nodes.
		// unsigned int shellIndex = 2;
//		cout << "dataslot = " << dataSlot << ", shellIndex = " << shellIndex << ", sendstr = " << sendstr << endl << flush;
		// char* buf = static_cast< char* >( pdata->innerGetAsyncParBuf( shellIndex, strlen( sendstr ) + 1 ));
		// strcpy( buf, sendstr );
	}
	MPI::COMM_WORLD.Barrier();
	bool glug = 0; // Breakpoint for parallel debugging
	while ( glug ) ;
	// cout << " starting string send\n" << flush;
	set< double >( cj, "start", 1.0 );
	// cout << " Done string send\n" << flush;
	MPI::COMM_WORLD.Barrier();

	////////////////////////////////////////////////////////////////
	// Now we set up a fully connected network of tables. Each 
	// node has numNodes tables. The one with index myNode is a source
	// the others are destinations. On each node the myNode table 
	// sends messages to tables with the same index on other nodes, 
	// to transfer its data to them. 
	//
	// We manually set up the msgs from
	// tables to postmaster and from postmaster to tables
	// This tests the full message flow process.
	// It also tests that info goes to multiple targets correctly.
	// It also tests multiple time-step transfer.
	//
	// We have to be careful that ordering of messages matches on
	// src and dest nodes. This works if we first create messages
	// with the lower table index, regardless of whether the
	// message is to or from the table.
	////////////////////////////////////////////////////////////////
	
	vector< Element* > tables( numNodes, 0 );
	Id tickId;
	lookupGet< Id, string >( cj, "lookupChild", tickId, "t0" );
	Element* tick = tickId();
	const Finfo* tickProcFinfo = tick->findFinfo( "outgoingProcess" );
	assert( tickProcFinfo != 0 );
	for ( i = 0; i < numNodes; i++ ) {
		char tabname[20];
		sprintf( tabname, "tab%d", i );
		tables[ i ] = Neutral::create( "Table", tabname, n->id(), Id::scratchId() );
		ASSERT( tables[i] != 0, "Checking data flow" );
		// const Finfo* outFinfo = tables[i]->findFinfo( "outputSrc" );
		// const Finfo* inFinfo = tables[i]->findFinfo( "input" );
		set< int >( tables[i], "xdivs", 10 );
		set< double >( tables[i], "xmin", 0.0 );
		set< double >( tables[i], "xmax", 10.0 );
		set< double >( tables[i], "input", 0.0 );
		bool ret = tickId.eref().add( "outgoingProcess", 
			tables[i], "process" );
		ASSERT( ret, "scheduling tables" );

		if ( i == myNode ) { // This is source table
			set< int >( tables[i], "stepmode", 2 ); // TAB_ONCE
			set< double >( tables[i], "stepsize", 1.0 ); // TAB_ONCE
			for ( unsigned int k = 0; k <= 10; k++ )
				lookupSet< double, unsigned int >( 
								tables[i], "table", i * 10 + k, k );

			for ( unsigned int j = 0; j < numNodes; j++ ) {
				if ( j == myNode ) continue;
				Element* p = postId[j]();
				// const Finfo* dataFinfo = p->findFinfo( "data" );
				set< Id >( p, "targetId", tables[i]->id() );
				set< string >( p, "targetField", "msgInput" );
				// bool ret = outFinfo->add( tables[i], p, dataFinfo );
				ret = Eref( tables[i] ).add( "outputSrc", 
					p, "data" );
				ASSERT( ret, "Making input message to postmaster" );
	MPI::COMM_WORLD.Barrier();
			}
		} else {
			post = postId[i]();
			// const Finfo* dataFinfo = post->findFinfo( "data" );
			set< int >( tables[i], "stepmode", 3 ); // TAB_BUF
			set< double >( tables[i], "output", 0.0 ); // TAB_BUF
			for ( unsigned int k = 0; k <= 10; k++ )
				lookupSet< double, unsigned int >( 
								tables[i], "table", 0.0, k );

			ret = postId[i].eref().add( "data", tables[i], "input" );
			// bool ret = dataFinfo->add( post, tables[i], inFinfo );
			ASSERT( ret, "Making output message from postmaster" );
		}
	}
	set< double >( cj, "start", 11.0 );
	MPI::COMM_WORLD.Barrier();
	// At this point the contents of the tables should be changed by the
	// arrival of data.
	double value;
	for ( i = 0; i < numNodes; i++ ) {
		for ( unsigned int k = 0; k <= 10; k++ ) {
			lookupGet< double, unsigned int >( 
							tables[i], "table", value, k );
			// cout << "value = " << value << ", i = " << i << ", j = " << k << endl;
			if ( i == myNode ) {
				ASSERT( value == i * 10 + k , "Testing data transfer\n" );
			} else if ( k == 0 ) { // The value is delayed by one, and the first is zero
				ASSERT( value == 0 , "Testing data transfer\n" );
			} else {
				ASSERT( value == i * 10 + k - 1, "Testing data transfer\n");
			}
		}
	}
#endif // TABLE_DATA
	set( n, "destroy" );
	MPI::COMM_WORLD.Barrier();

	Id shellId( "/shell", "/" );
	Element* shell = shellId();
	if ( myNode == 0 ) {
		testMess( shell, numNodes );
	}
}
#endif

#endif
