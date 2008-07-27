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
// #include "ParShell.h"
//
static const Slot getSlot = 
	initShellCinfo()->getSlot( "parallel.getSrc" );
static const Slot respondToGetSlot = 
	initShellCinfo()->getSlot( "parallel.respondToGet" );
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

/**
 * This is called on the master node. For now we can get by with the
 * implicit node info
 */
bool Shell::addSingleMessage( const Conn* c, Id src, string srcField, 
	Id dest, string destField )
{
	Shell* sh = static_cast< Shell* >( c->data() );
	unsigned int srcNode = src.node();
	cout << "in Shell::addSingleMessage, src=" << src << "." << srcNode <<
		", srcField = " << srcField <<
		", dest = " << dest << "." << dest.node() << 
		", destField = " << destField << endl << flush;
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

	cout << "in Shell::addParallelSrc on node=" << sh->myNode_ << 
		", src=" << src << "." << src.node() <<
		", srcField = " << srcField <<
		", dest = " << dest << "." << dest.node() << 
		", destField = " << destField << endl << flush;

#ifdef DO_UNIT_TESTS
	Eref de = sh->getPost( destNode );
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
	cout << "In Shell::addParallelDest on " << sh->myNode_ << endl << flush;

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
		sendBack3< string, Id, Id > ( c, parMsgErrorSlot, 
			errMsg, src, dest );
			return;
	}

	unsigned int srcNode = src.node();
	int tgtMsg = tgtFinfo->msg();
	bool ret = setupProxyMsg( srcNode, src, asyncFuncId, dest, tgtMsg );
	if ( ret ) {
		sendBack2< Id, Id > ( c, parMsgOkSlot, src, dest );
	}
	assert( ret );
}

Eref Shell::getPost( unsigned int node ) const
{
	return Eref( post_, node );
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
