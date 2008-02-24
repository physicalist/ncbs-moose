/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include <iostream>
#include <map>
#include "Cinfo.h"
#include "SimpleElement.h"
#include "Send.h"
#include "DynamicFinfo.h"
#include "SetConn.h"
#include "ProcInfo.h"
#include "DestFinfo.h"
#include "DerivedFtype.h"
#include "SharedFtype.h"
#include "LookupFinfo.h"
// #include "LookupFtype.h"


DynamicFinfo::~DynamicFinfo()
{
	if ( generalIndex_ != 0 ) {
		// Assume that the ftype knows what to do here.
		ftype()->destroyIndex( generalIndex_ );
	}
}

DynamicFinfo* DynamicFinfo::setupDynamicFinfo(
	Element* e, const string& name, const Finfo* origFinfo,
	GetFunc getFunc, void* index )
{
	assert( e != 0 );


	// Note that we create this with a null index. This is because
	// the DynamicFinfo is a temporary and we don't want to lose
	// the index when we destroy it.
	DynamicFinfo* ret = new DynamicFinfo( name, origFinfo, getFunc, 0);

	// Here we check if there is a vacant Dynamic Finfo to use
	vector< Finfo* > flist;
	vector< Finfo* >::iterator i;
	e->listLocalFinfos( flist );
	for ( i = flist.begin(); i != flist.end(); i++ ) {
		DynamicFinfo* df = dynamic_cast< DynamicFinfo* >( *i );
		if ( df ) {
			// If this DynamicFinfo is already handling the origFinfo, 
			// just reuse it, but check that the index is the same too.
			if ( df->origFinfo_ == origFinfo && 
				df->generalIndex_ == index ) {
				delete ret;
				return df;
			}
			if ( e->msg( df->msg() )->size() == 0 ) {
				ret->msg_ = df->msg_;
				if ( df->generalIndex_ != 0 ) {
					df->ftype()->destroyIndex( df->generalIndex_ );
				}

				*df = *ret;
				df->generalIndex_ = index;
				delete ret;
				return df;
			}
		}
	}

	// Nope, we have to use the new DynamicFinfo.
	ret->generalIndex_ = index;
	e->addFinfo( ret );
	return ret;
}

/**
 * Here the DynamicFinfo intercepts add requests that would have
 * initiated from the original ValueFinfo or whatever it is 
 * substituting for.
 * Returns true on success.
 * It handles two cases:
 * - Just sending out a value to a target
 * - A shared message where it receives a trigger and also sends a
 *   value out to the target.
 */
bool DynamicFinfo::add( 
		Element* e, Element* destElm, const Finfo* destFinfo) const
{
		unsigned int srcFuncId;
		unsigned int destFuncId;
		unsigned int destMsg;
		unsigned int numDest;

		// How do we know what the target expects: a simple message
		// or a shared one? Here we use the respondToAdd to query it.
		
		if ( destFinfo->respondToAdd( destElm, e, ftype(),
								srcFuncId, destFuncId,
								destMsg, numDest ) )
		{
			assert( numDest == 1 );
			// First we handle the case where this just sends out its
			// value to the target.
			return Msg::add( e, destElm, msg_, destMsg );
		} else {
			// Here we make a SharedFtype on the fly for passing in the
			// respondToAdd.
			static DestFinfo trigFinfo( "trig", Ftype0::global(), &dummyFunc );
			Finfo* shared[] = { &trigFinfo, const_cast< Finfo* >( 
				origFinfo_ ) };
			SharedFtype sf ( shared, 2 );
			assert ( FuncVec::getFuncVec( srcFuncId )->size() == 0 );
			/*
			srcFuncId = FuncVec::getFuncVec( origFinfo_->funcId() )->trigFuncId();
			if ( destFinfo->respondToAdd( destElm, e, &sf,
								srcFuncId, destFuncId,
								destMsg, numDest ) ) {
				// This is the SharedFinfo case where the incoming
				// message should be a Trigger to request the return.
				unsigned int originatingConn =
					e->insertConnOnSrc( msg_, destFl, 0, 0);
				// Here we know that the target is source to the
				// trigger message. So its Conn must be on
				// its MsgSrc vector.
				unsigned int targetConn =
					destElm->insertConnOnSrc( destIndex, srcFl, 0, 0 );
				e->connect( originatingConn, destElm, targetConn );
				return 1;
			}
			*/
		}
		return 0;
}


/**
 * The DynamicFinfo, if it exists, must intercept operations directed
 * toward the original ValueFinfo. In this case it is able to
 * deal with message requests.
 * This Finfo must handle three kinds of requests:
 * - To assign a value: a set
 * - A request to extract a value: a trigger to send back values to
 *   the destinations from this DynamicFinfo.
 * - A sharedFinfo request: Set up both the trigger and the return.
 */
bool DynamicFinfo::respondToAdd(
					Element* e, Element* src, const Ftype *srcType,
					unsigned int& srcFuncId, unsigned int& returnFuncId,
					unsigned int& destIndex, unsigned int& numDest
) const
{
	assert ( src != 0 && e != 0 );

	// Handle assignment message inputs when ftype is the the same
	// as the original Finfo
	const FuncVec* fv = FuncVec::getFuncVec( srcFuncId );
	if ( srcType->isSameType( ftype() ) && fv->size() == 0 ) {
		returnFuncId = origFinfo_->funcId();
		destIndex = msg_;
		numDest = 1;
		return 1;
	}

	// Handle trigger message inputs. The trigger function was
	// passed in at creation time if the originating object was
	// a ValueFinfo. Otherwise it should be a dummyFunc. The
	// function will check for a dummyFunc and complain if it is
	// found.
	/**
	 * \todo: need to figure out what to do here.
	if ( Ftype0::isA( srcType ) && fv->size() == 0 )
	{
		if ( trigFunc_ == dummyFunc )
			return 0;
		returnFuncId = orig->funcId()->trigId;
		returnFl.push_back( trigFunc_ );
		destIndex = destIndex_;
		numDest = 1;
		return 1;
	}
	*/

	// Handle SharedFinfo requests. The srcFl should have one
	// RecvFunc designed to handle the returned value. The
	// src Ftype is a SharedFtype that we will have to match.
	if ( FuncVec::getFuncVec( srcFuncId )->size() == 1 )
	{
	/**
	 * \todo: need to figure out what to do here.
		if ( trigFunc_ == dummyFunc )
			return 0;

		TypeFuncPair respondArray[2] = {
			TypeFuncPair( ftype(), 0 ),
			TypeFuncPair( Ftype0::global(), trigFunc_ )
		};
		SharedFtype sf( respondArray, 2 );
		if ( sf.isSameType( srcType ) ) {
			returnFl.push_back( trigFunc_ );
			// We have to put this end of the message into the
			// MsgSrc array because there is a MsgSrc for the value.
			destIndex = srcIndex_;
			// I don't think numDest is used.
			numDest = 0;
			return 1;
		}
	*/
	}
	return 0;
}

Finfo* DynamicFinfo::copy() const
{
	void* ge = 0;
	if ( generalIndex_ != 0 ) {
		ge = ftype()->copyIndex( generalIndex_ );
	}
	DynamicFinfo* ret = new DynamicFinfo( *this );
	ret->generalIndex_ = ge;
	return ret;
}

/**
* The Ftype of the OrigFinfo knows how to do this conversion.
*/
bool DynamicFinfo::strSet( Element* e, const std::string &s ) const
{
	return ftype()->strSet( e, this, s );
}
			
// The Ftype handles this conversion.
bool DynamicFinfo::strGet( const Element* e, std::string &s ) const {
	return ftype()->strGet( e, this, s );
}


/**
 * The DynamicFinfo is able to handle both MsgSrc and MsgDest,
 * so it books a place in both. 
 * The ConnIndex is used to identify the DynamicFinfo from
 * incoming RecvFuncs. This must, of course, be kept updated
 * in the event of a change in the conn_ vector.
 * \todo: Update ConnIndex_ if there is a change in conn_
 */
///\todo this needs to be defined according to the funcs.
void DynamicFinfo::countMessages( unsigned int& num )
{
	msg_ = num++;
}

const Finfo* DynamicFinfo:: match( Element* e, const string& n ) const 
{
	if ( n == name() )
		return this;
	return 0;
}

/**
 * Returns self if the specified Conn is managed by this Finfo.
 * The DynamicFinfo::match is likely to be used a lot because
 * any Finfo that doesn't have a compiled-in MsgSrc or MsgDest
 * uses DynamicFinfos, which have to be looked up using this
 * function. The problem is that the isConnOnSrc function is
 * a little tedious. 
 * \todo Need to do benchmarking to see if this needs optimising
 */
const Finfo* DynamicFinfo::match( 
				const Element* e, const ConnTainer* c) const
{
	const Msg* m = e->msg( msg() );
	if ( m->isDest() ) {
		if ( c->e2() == e && c->msg2() == msg() )
			return this;
	} else {
		if ( c->e1() == e && c->msg1() == msg() )
			return this;
	}
	return 0;
}	
	

/*
void* DynamicFinfo::traverseIndirection( void* data ) const
{
	if ( indirect_.size() == 0 )
			return 0;
	vector< IndirectType >::const_iterator i;
	for ( i = indirect_.begin(); i != indirect_.end(); i++ )
			data =  i->first( data, i->second );
	return data;
}
*/

const DynamicFinfo* getDF( const Conn* c )
{
	// The MAXUINT index is used to show that this conn is a dummy
	// one and must not be used for finding DynamicFinfos.
	assert( c->targetIndex() != MAXUINT );
	Element* e = c->targetElement();
	const Msg* m = e->msg( c->targetMsg() );

	return 0;

	/*
	const Finfo* temp = e->findFinfo( c->targetIndex() );
	const DynamicFinfo* f = dynamic_cast< const DynamicFinfo* >( temp );
	assert( f != 0 );
	return f;
	*/
}

bool DynamicFinfo::getSlot( const string& name, Slot& ret ) const
{
	if ( name != this->name() ) return 0;
	if ( msg_ != 0 )
		ret = Slot( msg_, 0 );
	return 1;
}
