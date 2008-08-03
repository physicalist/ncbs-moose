/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifdef USE_MPI

#include "moose.h"
#include "IdManager.h"
#include "../element/Neutral.h"
#include "../element/Wildcard.h"
#include "Shell.h"
#include "ReadCell.h"
#include "SimDump.h"

extern void pollPostmaster(); // Defined in maindir/mpiSetup.cpp

static const Slot getSlot = 
	initShellCinfo()->getSlot( "parallel.getSrc" );
static const Slot returnGetSlot = 
	initShellCinfo()->getSlot( "parallel.returnGetSrc" );
static const Slot setSlot = 
	initShellCinfo()->getSlot( "parallel.setSrc" );
static const Slot pCreateSlot =
	initShellCinfo()->getSlot( "parallel.createSrc" );

static const Slot addLocalSlot = 
	initShellCinfo()->getSlot( "parallel.addLocalSrc" );
static const Slot addParallelSrcSlot = 
	initShellCinfo()->getSlot( "parallel.addParallelSrcSrc" );
static const Slot addParallelDestSlot = 
	initShellCinfo()->getSlot( "parallel.addParallelDestSrc" );

static const Slot parMsgErrorSlot = 
	initShellCinfo()->getSlot( "parallel.parMsgErrorSrc" );
static const Slot parMsgOkSlot = 
	initShellCinfo()->getSlot( "parallel.parMsgOkSrc" );

static const Slot parTraversePathSlot = 
	initShellCinfo()->getSlot( "parallel.parTraversePathSrc" );
static const Slot returnParTraverseSlot = 
	initShellCinfo()->getSlot( "parallel.parTraversePathReturnSrc" );

static const Slot requestLeSlot = 
	initShellCinfo()->getSlot( "parallel.requestLeSrc" );
static const Slot returnLeSlot = 
	initShellCinfo()->getSlot( "parallel.returnLeSrc" );

static const Slot requestPathSlot = 
	initShellCinfo()->getSlot( "parallel.requestPathSrc" );
static const Slot returnPathSlot = 
	initShellCinfo()->getSlot( "parallel.returnPathSrc" );



/**
 * Manages the setup of a message emanating from this postmaster to 
 * one or more targets. This is a PostMaster operation because we need to
 * set up assorted proxies.
 *
 * This rather nasty function does a bit both of SrcFinfo::add and
 * DestFinfo::respondToAdd, since it has to bypass much of the logic of
 * both.
 */
extern bool setupProxyMsg( 
	unsigned int srcNode, Id proxy, unsigned int srcFuncId, 
	Id dest, int destMsg );


void printNodeInfo( const Conn* c )
{
	Element* post = c->source().e;
	assert( post->className() == "proxy" );
	unsigned int mynode = Shell::myNode();
	// unsigned int remotenode;
	// get< unsigned int >( post, "localNode", mynode );
	// get< unsigned int >( post, "remoteNode", remotenode );

	// cout << "on " << mynode << " from " << remotenode << ":";
	cout << "on " << mynode << ":";
}

void Shell::parGetField( const Conn* c, Id id, string field, 
	unsigned int requestId )
{
	// printNodeInfo( c );
	// cout << "in slaveGetFunc on " << id << " with field :" << field << "\n";
	if ( id.bad() )
		return;
	string ret = "";
	Element* e = id();
	if ( e == 0 )
		return;

	const Finfo* f = e->findFinfo( field );
	if ( f ) {
		if ( f->strGet( e, ret ) ) {
			sendBack2< string, unsigned int >( c, returnGetSlot, ret,
				requestId );
			return;
		}
	} else {
		cout << "Shell::parGetField: Failed to find field " << field << 
			" on object " << id.path() << endl;
	}	
	// Have to respond anyway.
	sendBack2< string, unsigned int >( c, returnGetSlot, ret,
		requestId );
}

// was recvGetFunc.
// Takes the value and stuffs it into the appropriate place on the 
// offNode data manager. Tells the system that the job is done.
void Shell::handleReturnGet( const Conn* c,
	string value, unsigned int requestId )
{
	// printNodeInfo( c );
	// cout << "in recvGetFunc with field value :'" << value << endl << flush;
	// send off to parser maybe.
	// Problem if multiple parsers.
	// Bigger problem that this is asynchronous now.
	// Maybe it is OK if only one parser.
	// sendTo1< string >( c.targetElement(), getFieldSlot, 0, value );
	Shell* sh = static_cast< Shell* >( c->data() );
	*( getOffNodeValuePtr< string >( sh, requestId ) ) = value;
	sh->zeroOffNodePending( requestId );
}

/**
 * Creates a new object. Must be called on the same node as the
 * parent object.
 */
void Shell::parCreateFunc ( const Conn* c, 
				string objtype, string objname, 
				Nid parent, Nid newobj )
{
	// printNodeInfo( c );
	// cout << "in slaveCreateFunc :" << objtype << " " << objname << " " << parent << "." << parent.node() << " " << newobj << "." << newobj.node() << "\n";

	Shell* s = static_cast< Shell* >( c->data() );
	if ( parent == Id() || parent == Id::shellId() ) {
		parent.setNode( s->myNode_ );
	}

	assert ( s->myNode_ == parent.node() );
	// both parent and child are here. Straightforward.
	bool ret = 1;
	if ( parent.node() == newobj.node() ) {
		ret = s->create( objtype, objname, parent, newobj );
	} else {
		cout << "Shell::parCreateFunc: Currently cannot put child on different node than parent\n";
		// send message to create child on remote node. This includes
		// 	messaging from proxy to child.
		// set up local messaging to connect to child.
	}

	if ( ret ) { // Tell the master node it was created happily.
		// sendTo2< Id, bool >( e, createCheckSlot, c.targetIndex(), newobj, 1 );
	} else { // Tell master node that the create failed.
		// sendTo2< Id, bool >( e, createCheckSlot, c.targetIndex(), newobj, 0 );
	}
}

/**
 * This is called on the master node. For now we can get by with the
 * implicit node info
 */
bool Shell::addSingleMessage( const Conn* c, Id src, string srcField, 
	Id dest, string destField )
{
	Shell* sh = static_cast< Shell* >( c->data() );
	unsigned int srcNode = src.node();
	// cout << "in Shell::addSingleMessage, src=" << src << "." << srcNode << ", srcField = " << srcField << ", dest = " << dest << "." << dest.node() << ", destField = " << destField << endl << flush;
	if ( srcNode == sh->myNode_ ) {
		if ( dest.node() == sh->myNode_ ) {
			return addLocal( src, srcField, dest, destField );
		} else {
			addParallelSrc( c, src, srcField, dest, destField );
			return 1;
		}
	} else { // Off-node source. Deal with it remotely.
		unsigned int tgtMsg = 
			( srcNode <= sh->myNode_ ) ? srcNode: srcNode - 1;
		if ( dest.node() != srcNode ) {
			sendTo4< Nid, string, Nid, string >( 
				c->target(), addParallelSrcSlot, tgtMsg,
				src, srcField, dest, destField );
		} else {
			sendTo4< Id, string, Id, string >( 
				c->target(),
				addLocalSlot, tgtMsg,
				src, srcField, dest, destField );
		}
		return 1;
	}
	return 0;
}

/**
 * This is called from the same node that the src is on, to send a message
 * to a dest on a remote node. 
 * Note that an Id does not carry node info within itself. So we use an
 * Nid for the dest, as we need to retain node info.
 */
void Shell::addParallelSrc( const Conn* c,
	Nid src, string srcField, Nid dest, string destField )
{
	Shell* sh = static_cast< Shell* >( c->data() );
	//unsigned int srcNode = sh->myNode_;
	unsigned int destNode = dest.node();
	Eref se = src.eref();

	// cout << "in Shell::addParallelSrc on node=" << sh->myNode_ << ", src=" << src << "." << src.node() << ", srcField = " << srcField << ", dest = " << dest << "." << dest.node() << ", destField = " << destField << endl << flush;

#ifdef DO_UNIT_TESTS
	Eref de = sh->getPostForUnitTests( destNode );
	if ( de.e == 0 )
		de = Id::postId( destNode ).eref();
#else
	// One of the unit tests puts them on the same node.
	assert( destNode != srcNode );
	assert( Id::postId( destNode ).good() );
	assert( Id::postId( destNode ).eref().e != 0 );
	Eref de = Id::postId( destNode ).eref();
#endif

	const Finfo* sf = se->findFinfo( srcField );
	bool ret = 0;
	if ( sf->funcId() != 0 ) { 
		// If the src handles any funcs at all this will be nonzero.
		// In this case it must be a SharedFinfo
		// with some info coming back. So we set up a local proxy too.
		int srcMsg = sf->msg();
		ret = setupProxyMsg( destNode, 
			dest, sf->asyncFuncId(), 
			src, srcMsg );
	} else {
		ret = se.add( srcField, de, "async" );
		// bool ret = add2Post( destNode, se, srcField );
		// Need srcId to set up remote proxy
		// Need src Finfo type to do type checking across nodes.
		// Need destId to connect to target
		// Need destField to look up data types. Better check types first.
	}
	string srcFinfoStr = se->className() + "." + sf->name();
	if ( ret ) {
		unsigned int tgtMsg = 
			( destNode <= sh->myNode_ ) ? destNode : destNode - 1;
		sendTo4< Nid, string, Nid, string >( 
			c->target(), addParallelDestSlot, tgtMsg, 
			src, srcFinfoStr, dest, destField );
			
		// Set up an entry to check for completion. 
		sh->parMessagePending_[dest] = src; 
	} else {
		cout << "Error: Shell::addParallelSrc failed to set up msg from\n" <<
			src.path() << " to " << dest.path() << endl;
	}
}

const Finfo* findFinfoOnCinfo( const string& name )
{
	string::size_type pos = name.find( "." );
	if ( pos == string::npos )
		return 0;
	const Cinfo* c = Cinfo::find( name.substr( 0, pos ) );
	if ( c == 0 )
		return 0;
	return c->findFinfo( name.substr( pos + 1 ) );
}

void Shell::addParallelDest( const Conn* c,
	Nid src, string srcField, Nid dest, string destField )
{
	Shell* sh = static_cast< Shell* >( c->data() );
	// cout << "in Shell::addParallelDest on node=" << sh->myNode_ << ", src=" << src << "." << src.node() << ", srcField = " << srcField << ", dest = " << dest << "." << dest.node() << ", destField = " << destField << endl << flush;

	const Finfo* srcFinfo = findFinfoOnCinfo( srcField );
	const Finfo* tgtFinfo;
	unsigned int asyncFuncId = 0;

	string errMsg = "";

	if ( !srcFinfo )
		errMsg = "Src Field: '" + srcField + "' not found on remote node";

	if ( !( dest.good() && dest.node() == sh->myNode_ ) ) {
		errMsg = "Destination object not found on remote node ";
	} else {
		tgtFinfo = dest.eref()->findFinfo( destField );
		if ( tgtFinfo == 0 ) {
			errMsg = "Dest field: '" + destField + "' not found on remote node";
		} else {
			asyncFuncId = tgtFinfo->asyncFuncId();
			// Actually I should tap into respondToAdd here because it does
			// all the tests systematically and also handles messages to
			// fields. Only problem is that it needs the proxy element
			// to already be made.
			if ( !tgtFinfo->ftype()->isSameType( srcFinfo->ftype() ) )
				errMsg = "Type mismatch between srcField '" + srcField + 
				"' and destField '" + destField + "'";
		}
	}

	/*
	// Check for match of srcFinfo and tgtFinfo
	bool ret = tgtFinfo->respondToAdd( dest.eref(), src.eref(),
		srcFinfo->ftype(), srcFinfo->ftype()->asyncFuncId(), 
		destFid, destMsg, destIndex );
		*/
	
	if ( errMsg != "" ) {
		cout << "addParallelDest" << errMsg << endl << flush;
		/*
		sendBack3< string, Id, Id > ( c, parMsgErrorSlot, 
			errMsg, src, dest );
			return;
			*/
	}

	unsigned int srcNode = src.node();
	int tgtMsg = tgtFinfo->msg();
	bool ret = setupProxyMsg( srcNode, src, asyncFuncId, dest, tgtMsg );
	if ( ret ) {
		// sendBack2< Id, Id > ( c, parMsgOkSlot, src, dest );
	}
	assert( ret );
}

#ifdef DO_UNIT_TESTS
Eref Shell::getPostForUnitTests( unsigned int node ) const
{
	return Eref( post_, node );
}
#endif


/**
 * Sets off request for children on all remote nodes, starting at 
 * either root or shellId. Polls till all nodes return. Note that this
 * must be thread-safe, because during the polling there may be nested
 * calls.
 */
Id Shell::parallelTraversePath( Id start, vector< string >& names )
{
	assert( Id::shellId().good() );
	Shell* sh = static_cast< Shell* >( Id::shellId().eref().data() );

	// returns a thread-safe unique id
	// for the request, so that we can examine the return values for ones
	// we are interested in. This rid is an index to a vector of ints
	// that counts pending returns on this id. When we call for the
	// rid it sets up the pending returns to numNodes - 1.
	Nid ret( Id::badId() ); // Must pass in an initialized memory location
	unsigned int requestId = 
		openOffNodeValueRequest< Nid > ( sh, &ret, sh->numNodes() - 1 ); 

	// Send request to all nodes.
	send3< Id, vector< string >, unsigned int >( 
		Id::shellId().eref(), parTraversePathSlot,
		start, names, requestId );
	
	// Get the value back.
	Nid* temp = closeOffNodeValueRequest< Nid >( sh, requestId );
	assert ( &ret == temp );
	return ret;
}

void Shell::handleParTraversePathRequest( const Conn* c, 
	Id start, vector< string > names, unsigned int requestId )
{
	assert( start == Id() || start == Id::shellId() );
	Id ret = localTraversePath( start, names );
	sendBack2< Nid, unsigned int >( c, returnParTraverseSlot,
		ret, requestId );
}

/**
 * Undefined effects if more than one node has a matching target.
 */
void Shell::handleParTraversePathReturn( const Conn* c,
	Nid found, unsigned int requestId )
{
	Shell* sh = static_cast< Shell* >( c->data() );
	if ( !found.bad() ) { 
		// Got it!! But we need to hold on till everyone is back.
		*( getOffNodeValuePtr< Nid >( sh, requestId ) ) = found;
	}
	sh->decrementOffNodePending( requestId );
}

///////////////////////////////////////////////////////////////////////
// Here we handle 'le' requests.
///////////////////////////////////////////////////////////////////////

void Shell::handleRequestLe( const Conn* c, 
	Nid parent, unsigned int requestId )
{
	vector< Id > ret;
	bool flag = get< vector< Id > >( parent.eref(), "childList", ret );
	assert( flag );
	vector< Id >::iterator i;
	vector< Nid > temp;
	for ( i = ret.begin(); i != ret.end(); i++ )
		if ( i->node() != Id::GlobalNode )
			temp.push_back( *i );

	sendBack2< vector< Nid >, unsigned int >( c, returnLeSlot,
		temp, requestId );
}

/**
 * Undefined effects if more than one node has a matching target.
 */
void Shell::handleReturnLe( const Conn* c,
	vector< Nid > found, unsigned int requestId )
{
	Shell* sh = static_cast< Shell* >( c->data() );
	if ( found.size() > 0 ) {
		vector< Nid >* temp = 
			getOffNodeValuePtr< vector< Nid > >( sh, requestId );
		temp->insert( temp->end(), found.begin(), found.end() );
	}
	sh->decrementOffNodePending( requestId );
}

///////////////////////////////////////////////////////////////////////
// Here we handle wildcard building requests.
// Very similar to Le, and in fact we reuse the return handler.
///////////////////////////////////////////////////////////////////////

void Shell::handleParWildcardList( const Conn* c, 
	string path, bool ordered, unsigned int requestId )
{
	vector< Id > ret;
	vector< Nid > temp;
	innerGetWildcardList( c, path, ordered, ret );
	for ( vector< Id >::iterator i = ret.begin(); i != ret.end(); i++ )
		if ( i->node() != Id::GlobalNode )
			temp.push_back( *i );

	// Turns out to be identical return operations as handleReturnLe,
	// and the requestId
	// keeps things straight between calling functions.
	sendBack2< vector< Nid >, unsigned int >( c, returnLeSlot,
		temp, requestId );
}

////////////////////////////////////////////////////////////////////
// Here we put in stuff to deal with eid2path
////////////////////////////////////////////////////////////////////

string Shell::eid2path( Id eid )
{
	if ( !eid.isGlobal() && eid.node() != Shell::myNode() ) {
		Shell *sh = static_cast< Shell* >( Id::shellId().eref().data() );
		unsigned int tgt = ( eid.node() < Shell::myNode() ) ?
			eid.node() : eid.node() - 1;
		string ret = "";
		unsigned int requestId = 
			openOffNodeValueRequest< string > ( sh, &ret, 1 ); 
		// Send request to target node.
		sendTo2< Nid, unsigned int >( 
			Id::shellId().eref(), requestPathSlot, tgt, 
				eid, requestId );
		// Get the value back.
		string* temp = closeOffNodeValueRequest< string >( sh, requestId );
		assert ( &ret == temp );
		return ret;
	} else {
		return localEid2Path( eid );
	}
}

void Shell::handlePathRequest( const Conn* c, 
	Nid eid, unsigned int requestId )
{
	assert( eid.node() == myNode() );
	string ret = localEid2Path( eid );
	sendBack2< string, unsigned int >( c, returnPathSlot,
		ret, requestId );
}

void Shell::handlePathReturn( const Conn* c,
	string ret, unsigned int requestId )
{
	Shell* sh = static_cast< Shell* >( c->data() );
	string* temp = getOffNodeValuePtr< string >( sh, requestId );
	*temp = ret;
	sh->zeroOffNodePending( requestId );
}


////////////////////////////////////////////////////////////////////
// Here we put in the offNodeValueRequest stuff.
////////////////////////////////////////////////////////////////////

/**
 * The next two functions should always be called in pairs and should
 * be called within the same function, so that local variables do not
 * get lost.
 *
 * openOffNodeValueRequest:
 * Inner function to handle requests for off-node operations returning
 * values.
 * Returns a thread-safe unique id for the request, so that 
 * we can examine the return values for ones we are interested in. 
 * This rid is an index to a vector of ints that counts pending 
 * returns on this id.
 * Returns the next free Rid and initializes offNodeData_ entry.
 *
 * The init argument is typically a local variable whose value will
 * be read out in the succeeding extractOffNodeValue call. If these
 * are not in the same function, then the user has to use allocated
 * memory.
 */
unsigned int Shell::openOffNodeValueRequestInner( 
	void* init, unsigned int numPending )
{
	unsigned int ret = freeRidStack_.back();
	freeRidStack_.pop_back();
	if ( freeRidStack_.size() == 0 )
		cout << "Error: Shell::openOffNodeValueRequestInner(): Empty Rid stack\n";
	offNodeData_[ ret ].numPending = numPending;
	offNodeData_[ ret ].data = init;
	return ret;
}

/**
 * closeOffNodeValueRequest:
 * Polls postmaster, converts and returns data stored at rid
 */
void* Shell::closeOffNodeValueRequestInner( unsigned int rid )
{
	assert( rid < offNodeData_.size() );
	void* ret = offNodeData_[ rid ].data;
	assert( ret != 0 );
	while ( offNodeData_[rid].numPending > 0 )
		pollPostmaster();
	freeRidStack_.push_back( rid );
	offNodeData_[ rid ].data = 0;
	return ret;
}

void* Shell::getOffNodeValuePtrInner( unsigned int rid )
{
	assert( offNodeData_.size() > rid );
	assert( offNodeData_[ rid ].data != 0 ); 
	return offNodeData_[ rid ].data;
}

void Shell::decrementOffNodePending( unsigned int rid )
{
	assert( rid < offNodeData_.size() );
	assert( offNodeData_[ rid ].numPending > 0 );
	offNodeData_[rid].numPending--;
}

void Shell::zeroOffNodePending( unsigned int rid )
{
	assert( rid < offNodeData_.size() );
	offNodeData_[rid].numPending = 0;
}

unsigned int Shell::numPendingOffNode( unsigned int rid )
{
	assert( rid < offNodeData_.size() );
	return offNodeData_[rid].numPending;
}

/*
void ParShell::planarconnect(const Conn* c, string source, string dest, string spikegenRank, string synchanRank)
{
        int next, previous;
        bool ret;

        Id spkId(source);
        Id synId(dest);

        Element *eSpkGen = spkId();
        Element *eSynChan = synId();

        previous = 0;
        while(1)
        {
                next = spikegenRank.find('|', previous);
                if(next == -1)
                        break;


                ret = set< int >( eSpkGen, "sendRank", atoi(spikegenRank.substr(previous, next-previous).c_str()) );
                previous = next+1;
        }

        previous = 0;
        while(1)
        {
                next = synchanRank.find('|', previous);

                if(next == -1)
                        break;

                ret = set< int >( eSynChan, "recvRank", atoi(synchanRank.substr(previous, next-previous).c_str()) );
                previous = next+1;
        }

}

void ParShell::planardelay(const Conn* c, string source, double delay){
	vector <Element* > src_list;
	simpleWildcardFind( source, src_list );
	for (size_t i = 0 ; i < src_list.size(); i++){
		if (src_list[i]->className() != "ParSynChan"){cout<<"ParShell::planardelay: error!!";}
		unsigned int numSynapses;
		bool ret;
		ret = get< unsigned int >( src_list[i], "numSynapses", numSynapses );
		if (!ret) {cout << "error" <<endl;}
		for (size_t j = 0 ; j < numSynapses; j++){
			lookupSet< double, unsigned int >( src_list[i], "delay", delay, j );
		}
	}
}

void ParShell::planarweight(const Conn* c, string source, double weight){
	vector <Element* > src_list;
	simpleWildcardFind( source, src_list );
	for (size_t i = 0 ; i < src_list.size(); i++){
		if (src_list[i]->className() != "ParSynChan"){cout<<"ParShell::planarweight: error";}
		unsigned int numSynapses;
		bool ret;
		ret = get< unsigned int >( src_list[i], "numSynapses", numSynapses );
		if (!ret) {}
		for (size_t j = 0 ; j < numSynapses; j++){
			lookupSet< double, unsigned int >( src_list[i], "weight", weight, j );
		}
	}
}

*/
#endif // USE_MPI
