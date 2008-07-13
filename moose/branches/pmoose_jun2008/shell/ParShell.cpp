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

bool Shell::addSingleMessage( const Conn* c, Id src, string srcField, 
	Id dest, string destField )
{
	Shell* sh = static_cast< Shell* >( c->data() );
	unsigned int srcNode = src.node();
	if ( srcNode == sh->node_ ) {
		if ( dest.node() == sh->node_ ) {
			return addLocal( src, srcField, dest, destField );
		} else {
			addParallelSrc( c, src, srcField, dest, destField );
			return 1;
		}
	} else { // Off-node source. Deal with it remotely.
		if ( dest.node() != srcNode ) {
			sendTo4< Id, string, Id, string >( 
				c->target(), addParallelSrcSlot, srcNode,
				src, srcField, dest, destField );
		} else {
			sendTo4< Id, string, Id, string >( 
				c->target(),
				addLocalSlot, srcNode,
				src, srcField, dest, destField );
		}
		return 1;
	}
	return 0;
}

/**
 * This is called from the same node that the src is on, to send a message
 * to a dest on a remote node.
 */
void Shell::addParallelSrc( const Conn* c,
	Id src, string srcField, Id dest, string destField )
{
	Shell* sh = static_cast< Shell* >( c->data() );
	unsigned int srcNode = sh->node_;
	unsigned int destNode = dest.node();
	Eref se = src.eref();

#ifdef DO_UNIT_TESTS
	// One of the unit tests needs a unique id for the proxy.
	src = Id::scratchId();
#else // DO_UNIT_TESTS
	// One of the unit tests puts them on the same node.
	assert( destNode != srcNode );
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
		assert( ret );
	} else {
		ret = se.add( srcField, sh->getPost( destNode ), "async" );
		// bool ret = add2Post( destNode, se, srcField );
		// Need srcId to set up remote proxy
		// Need src Finfo type to do type checking across nodes.
		// Need destId to connect to target
		// Need destField to look up data types. Better check types first.
	}
	string srcFinfoStr = se->className() + "." + sf->name();
	if ( ret ) {
		sendTo4< Id, string, Id, string >( 
			c->target(), addParallelDestSlot, srcNode, 
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
	Id src, string srcField, Id dest, string destField )
{
	Shell* sh = static_cast< Shell* >( c->data() );

	const Finfo* srcFinfo = findFinfoOnCinfo( srcField );

	string errMsg = "";

	if ( !srcFinfo )
		errMsg = "Src Field: '" + srcField + "' not found on remote node";

	if ( !( dest.good() && dest.node() == sh->node_ ) )
		errMsg = "Destination object not found on remote node ";

	const Finfo* tgtFinfo = dest.eref()->findFinfo( destField );
	if ( tgtFinfo == 0 )
		errMsg = "Dest field: '" + destField + "' not found on remote node";

	unsigned int asyncFuncId = tgtFinfo->asyncFuncId();
	//unsigned int asyncFuncId = 0;

	// Actually I should tap into respondToAdd here because it does
	// all the tests systematically and also handles messages to fields.
	// Only problem is that it needs the proxy element to already be
	// made.
	if ( !tgtFinfo->ftype()->isSameType( srcFinfo->ftype() ) )
		errMsg = "Type mismatch between srcField '" + srcField + 
		"' and destField '" + destField + "'";

	/*
	// Check for match of srcFinfo and tgtFinfo
	bool ret = tgtFinfo->respondToAdd( dest.eref(), src.eref(),
		srcFinfo->ftype(), srcFinfo->ftype()->asyncFuncId(), 
		destFid, destMsg, destIndex );
		*/
	
	if ( errMsg != "" ) {
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
