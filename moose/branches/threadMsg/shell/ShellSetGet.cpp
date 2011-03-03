/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
/*
#include "MsgManager.h"
#include "SingleMsg.h"
#include "DiagonalMsg.h"
#include "OneToOneMsg.h"
#include "OneToAllMsg.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "ReduceMsg.h"
#include "ReduceFinfo.h"
*/
#include "AssignmentMsg.h"
#include "AssignVecMsg.h"
#include "ReduceBase.h"
#include "ReduceMax.h"
#include "Shell.h"
// #include "Dinfo.h"

////////////////////////////////////////////////////////////////////////
// Functions for handling field set/get and func calls
////////////////////////////////////////////////////////////////////////

void Shell::handleSet( const Eref& e, const Qinfo* q, 
	Id id, DataId d, FuncId fid, PrepackedBuffer arg )
{
	if ( q->addToStructuralQ() )
		return;
	Eref er( id(), d );
	shelle_->clearBinding ( lowLevelSetGet()->getBindIndex() );
	Eref sheller( shelle_, 0 );
	Msg* m;
	
	if ( arg.isVector() ) {
		m = new AssignVecMsg( Msg::setMsg, sheller, er.element() );
	} else {
		m = new AssignmentMsg( Msg::setMsg, sheller, er );
		// innerSet( er, fid, arg.data(), arg.dataSize() );
	}
	shelle_->addMsgAndFunc( m->mid(), fid, lowLevelSetGet()->getBindIndex() );
	if ( myNode_ == 0 )
		lowLevelSetGet()->send( sheller, &p_, arg );
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
	PrepackedBuffer buf( args, size );
	s->innerDispatchSet( sheller, tgt, fid, buf );
}

// regular function, does the actual dispatching.
void Shell::innerDispatchSet( Eref& sheller, const Eref& tgt, 
	FuncId fid, const PrepackedBuffer& buf )
{
	Id tgtId( tgt.element()->id() );
	initAck();
		requestSet()->send( sheller, &p_,  tgtId, tgt.index(), fid, buf );
	waitForAck();
}

// Static function.
void Shell::dispatchSetVec( const Eref& tgt, FuncId fid, 
	const PrepackedBuffer& pb )
{
	Eref sheller = Id().eref();
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	s->innerDispatchSet( sheller, tgt, fid, pb );
}

/**
 * Returns buffer containing desired data.
 * Static function, used for developer-code triggered SetGet functions.
 * Should only be issued from master node.
 * This is a blocking function and returns only when the job is done.
 */
const vector< char* >& Shell::dispatchGet( 
	const Eref& e, const string& field, 
	const SetGet* sg, unsigned int& retEntries )
{
	static vector< char* > badRet( 1 );
	badRet[0] = 0;
	Eref tgt( e );
	retEntries = 0; // in case function fails.
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
				return badRet;
			}
		}
	}
	const DestFinfo * df = dynamic_cast< const DestFinfo* >( gf );
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	if ( !df ) {
		cout << s->myNode() << ": Error: Shell::dispatchGet: field '" << field << "' not found on " << tgt << endl;
		return badRet;
	}

	if ( df->getOpFunc()->checkSet( sg ) ) { // Type validation
			Eref sheller = Id().eref();

			// Later we need to fine-tune to handle lookup of fields on
			// one object, or a specific field on any object, or even
			// sub-dimensions. For now, just the whole lot.
			if ( tgt.index() == DataId::any() )
				retEntries = tgt.element()->dataHandler()->totalEntries();
			else
				retEntries = 1;
			return s->innerDispatchGet( sheller, tgt, df->getFid(),
				retEntries );
	} else {
		cout << s->myNode() << ": Error: Shell::dispatchGet: type mismatch for field " << field << " on " << tgt << endl;
	}
	return badRet;
}

/**
 * Tells all nodes to dig up specified field, if object is present on node.
 * Not thread safe: this should only run on master node.
 */
const vector< char* >& Shell::innerDispatchGet( 
	const Eref& sheller, const Eref& tgt, 
	FuncId fid, unsigned int retEntries )
{
	// static timespec sleepTime = { 0, 10000}; // 0.1 msec.
	// static timespec sleepRem;
	clearGetBuf();
	gettingVector_ = (retEntries > 1 );
	getBuf_.resize( retEntries );
	numGetVecReturns_ = 0;
	initAck();
		requestGet()->send( sheller, &p_, tgt.element()->id(), tgt.index(), 
			fid, retEntries );
	waitForGetAck();

	assert( getBuf_.size() == retEntries );

	return getBuf_;
}


/**
 * This operates on the worker node. It handles the Get request from
 * the master node, and dispatches if need to the local object.
 */
void Shell::handleGet( const Eref& e, const Qinfo* q, 
	Id id, DataId index, FuncId fid, unsigned int numTgt )
{
	if ( q->addToStructuralQ() )
		return;

	Eref sheller( shelle_, 0 );
	Eref tgt( id(), index );
	FuncId retFunc = receiveGet()->getFid();

	shelle_->clearBinding( lowLevelSetGet()->getBindIndex() );
	if ( numTgt > 1 ) {
		Msg* m = new AssignVecMsg( Msg::setMsg, sheller, tgt.element() );
		shelle_->addMsgAndFunc( m->mid(), fid, lowLevelSetGet()->getBindIndex() );
		if ( myNode_ == 0 ) {
			//Need to find numTgt
			vector< FuncId > rf( numTgt, retFunc );
			const char* temp = reinterpret_cast< const char* >( &rf[0] );
			PrepackedBuffer pb( temp, sizeof( FuncId ), numTgt );
			lowLevelSetGet()->send( sheller, &p_, pb );
		}
	} else {
		Msg* m = new AssignmentMsg( Msg::setMsg, sheller, tgt );
		shelle_->addMsgAndFunc( m->mid(), fid, lowLevelSetGet()->getBindIndex());
		if ( myNode_ == 0 ) {
			PrepackedBuffer pb( 
				reinterpret_cast< const char* >( &retFunc), 
				sizeof( FuncId ) );
			lowLevelSetGet()->send( sheller, &p_, pb );
		}
	}
}

void Shell::recvGet( const Eref& e, const Qinfo* q, PrepackedBuffer pb )
{
	if ( myNode_ == 0 ) {
		if ( gettingVector_ ) {
			assert( q->mid() != Msg::badMsg );
			Element* tgtElement = Msg::getMsg( q->mid() )->e2();
			Eref tgt( tgtElement, q->srcIndex() );
			assert ( tgt.linearIndex() < getBuf_.size() );
			char*& c = getBuf_[ tgt.linearIndex() ];
			c = new char[ pb.dataSize() ];
			memcpy( c, pb.data(), pb.dataSize() );
			// cout << myNode_ << ": Shell::recvGet[" << tgt.linearIndex() << "]= (" << pb.dataSize() << ", " <<  *reinterpret_cast< const double* >( c ) << ")\n";
		} else  {
			assert ( getBuf_.size() == 1 );
			char*& c = getBuf_[ 0 ];
			c = new char[ pb.dataSize() ];
			memcpy( c, pb.data(), pb.dataSize() );
		}
		++numGetVecReturns_;
	}
}

/*
void Shell::lowLevelRecvGet( PrepackedBuffer pb )
{
	cout << "Shell::lowLevelRecvGet: If this is being used, then fix\n";
	relayGet.send( Eref( shelle_, 0 ), &p_, myNode(), OkStatus, pb );
}
*/

////////////////////////////////////////////////////////////////////////

void Shell::clearGetBuf()
{
	for ( vector< char* >::iterator i = getBuf_.begin(); 
		i != getBuf_.end(); ++i )
	{
		if ( *i != 0 ) {
			delete[] *i;
			*i = 0;
		}
	}
	getBuf_.resize( 1, 0 );
}
