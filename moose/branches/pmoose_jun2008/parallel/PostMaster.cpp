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
	requestFlag_( 0 ),
	comm_( &MPI::COMM_WORLD )
{
	localNode_ = MPI::COMM_WORLD.Get_rank(); 
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
#endif // DO_UNIT_TESTS
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

	// cout << "In setupProxyMsg, ret= " << ret << ", proxy=" << proxy << ", dest = " << dest << ", destFuncId = " << destFuncId << endl;
	
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
//	cout << "inner PostIrecv on node " << localNode_ << " from " << remoteNode_ << endl << flush;
	if ( localNode_ != remoteNode_ ) {
		if ( requestFlag_ ) { // Not used last time, recycle.
			donePoll_ = 0;
		} else {
			request_ = comm_->Irecv(
				&( recvBuf_[0] ), recvBuf_.size(), MPI_CHAR, 
					remoteNode_, DATA_TAG );
			requestFlag_ = 1;
			// cout << inBufSize_ << " innerPostIrecv: request_ empty?" << ( request_ == static_cast< MPI::Request >( 0 ) ) << "\n";
			donePoll_ = 0;
		}
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
	// cout << "inner Poll on node " << localNode_ << " from " << remoteNode_ << endl << flush;
	if ( donePoll_ )
			return;
	if ( !requestFlag_ ) {
		sendBack1< unsigned int >( c, pollSlot, remoteNode_ );
		donePoll_ = 1;
		return;
	}
	if ( request_.Test( status_ ) ) {
		// I believe that when this is true, the request is deallocated.
		// MPI documentation is obscure on this point.
		
		// Data has arrived. How big was it?
		unsigned int dataSize = status_.Get_count( MPI_CHAR );
		// cout << dataSize << " bytes of data arrived on " << localNode_ << " from " << remoteNode_;
		requestFlag_ = 0;
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
		// cout << ", nMsgs = " << nMsgs << ", proxyId = " << foo.proxy() << ", dataSize = " << foo.size() << ", func = " << foo.funcIndex() << endl;

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
	// cout << "Barrier on node " << localNode_ << " from " << remoteNode_ << endl << flush;
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
	// cout << "sending " << sendBufPos_ << " bytes: " << &sendBuf_[0] << " from node " << localNode_ << " to " << remoteNode_;
	if ( localNode_ != remoteNode_ && numSendBufMsgs_ > 0 ) {
		comm_->Send( data, sendBufPos_, MPI_CHAR, remoteNode_, DATA_TAG );

		/*
		unsigned int nMsgs = *static_cast< const unsigned int* >(
						static_cast< const void* >( data ) );
		data += sizeof( unsigned int );
		AsyncStruct foo( data );
		cout << ", nMsgs = " << nMsgs << ", proxyId = " << foo.proxy() << ", dataSize = " << foo.size() << ", func = " << foo.funcIndex() << endl;
		*/

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

#endif // USE_MPI
