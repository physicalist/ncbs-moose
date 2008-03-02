/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
**           copyright (C) 2003-2008 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "SimpleConn.h"
#include "Msg.h"

Msg::Msg()
	: fv_( FuncVec::getFuncVec( 0 ) ), next_( 0 )
{;}

/**
 * Originally the ~Msg performed a dropAll. However, every time we 
 * resize the Msg vector we delete and recreate messages, so this is
 * wrong. Instead we need to do the dropAll explicitly when the object
 * is being deleted, but not when the individual msg is.
 */
Msg::~Msg()
{ ; }

void Msg::assignMsgByFuncId( 
	Element* e, unsigned int funcId, ConnTainer* ct )
{
	if ( fv_->id() == 0 )
		fv_ = FuncVec::getFuncVec( funcId );

	if ( fv_->id() == funcId ) {
		c_.push_back( ct );
		return;
	}

	if ( next_ )
		// do stuff here.
		;
	cout << "next msg stuff not yet working in Msg::assignMsgByFuncId\n";
}

/**
* Add a new message from e1 (source ) to e2 (dest).
* The m1 and m2 indicate source and dest msg indices.
* The funcId1 is the source funcId, which is going to be used
* at the dest, but is optional so it may be zero.
* The funcId2 is the dest funcId, which must be nonzero and will
* be used when the source calls the dest.
* Later I may relax the directional restrictions.
*/
ConnTainer* Msg::add( Element* e1, Element* e2, 
	unsigned int m1Index, unsigned int m2Index,
	unsigned int funcId1, unsigned int funcId2 )
{
	// Must always have a nonzero func on the destination
	assert( funcId2 != 0 );

	Msg* m1 = e1->varMsg( m1Index );
	if ( !m1 ) return 0;

	Msg* m2 = e2->varMsg( m2Index );
	if ( !m2 ) return 0;

	// Here need to check that the funcId matches.
	SimpleConnTainer* ct = new SimpleConnTainer( e1, e2, m1Index, m2Index );

	m1->assignMsgByFuncId( e1, funcId2, ct );
	m2->assignMsgByFuncId( e2, funcId1, ct );
	return ct;
}


/*
ConnTainer* Msg::addAll2All( Element* e1, Element* e2, 
	unsigned int m1Index, unsigned int m2Index )
{
	Msg* current = e1->msg( m1Index );
	if ( !current ) return 0;

	Msg* other = e2->msg( m2Index );
	if ( !other ) return 0;

	All2AllConnTainer* ct = 
		new All2AllConnTainer( e1, e2, m1Index, m2Index );
	current->c_.push_back( ct );
	other->c_.push_back( ct );
	return ct;
}
*/


/**
 * drop eliminates an identified ConnTainer.
 * This is NOT the call to initiate removing a connection.
 * It is called on the other end of the message from the one directly
 * set up for deletion, and assumes that the rest of the message will
 * be taken care of by the initiating function.
 * Assumes that the element checks first to see if it is also doomed.
 * If the element is to survive, only then it goes through the bother
 * of erasing.
 */
bool Msg::drop( const ConnTainer* doomed )
{
	vector< ConnTainer* >::iterator pos = 
		find( c_.begin(), c_.end(), doomed );
	if ( pos != c_.end() ) {
		///\todo Here we have to fix the 'next' message too.
		c_.erase( pos );
		return 1;
	}
	cout << "Msg::drop( const ConnTainer* doomed ): can't find doomed\n"; 
	return 0;
}

/**
 * This variant of drop initiates the removal of a specific ConnTainer,
 * identified by its index within the Msg. 
 * Note... doesn't yet handle next Msg.
 */
bool Msg::drop( unsigned int doomed )
{
	if ( doomed < c_.size() ) {
		ConnTainer* d = c_[ doomed ];
		Msg* remoteMsg;
		if ( fv_->isDest() ) // The current msg is the source
			remoteMsg = d->e2()->varMsg( d->msg2() );
		else 
			remoteMsg = d->e1()->varMsg( d->msg1() );

		assert( remoteMsg != 0 );
		if ( remoteMsg->drop( d ) ) {
			c_.erase( c_.begin() + doomed );
			delete d;
			return 1;
		} else {
			cout << "Error: Msg::drop( unsigned int doomed ): remoteMsg failed to drop\n"; 
		}
		return 0;
	}
	cout << "Msg::drop( unsigned int doomed ): doomed outside range\n"; 
	return 0;
}

/**
 * dropAll cleans up the entire set of messages. Typically used when the
 * Element is being deleted.
 */
void Msg::dropAll()
{
	vector< ConnTainer* >::iterator i;
	Msg* remoteMsg;

	for ( i = c_.begin(); i != c_.end(); i++ ) {
		if ( fv_->isDest() ) // The current msg is source.
			remoteMsg = ( *i )->e2()->varMsg( ( *i )->msg2() );
		else 
			remoteMsg = ( *i )->e1()->varMsg( ( *i )->msg1() );
		assert( remoteMsg != 0 );
		assert( remoteMsg != this );

		bool ret = remoteMsg->drop( *i );
		if ( ret )
			delete( *i );
		else
			cout << "Error: Msg::dropAll(): remoteMsg failed to drop\n"; 
	}
}

unsigned int Msg::size() const
{
	return c_.size();
}

/**
 * Some issues with this implementation for arrays
 */
Conn* Msg::findConn( unsigned int eIndex, unsigned int tgt ) const
{
	vector< ConnTainer* >::const_iterator i;
	for ( i = c_.begin(); i != c_.end(); i++ ) {
		if ( tgt >= ( *i )->size() ) {
			tgt -= ( *i )->size();
		} else {
			return ( *i )->conn( eIndex, fv_->isDest(), tgt );
		}
	}
	return 0;
}

/**
* True if this is the nominal destination of a message.
* The definition of message source and dest is done at Finfo
* setup time. For simple messages no problem. For Shared Finfos,
* the one that has the first 'source' entry is the source.
*
* Note that the value is the complement of funcVec flag, because
* the funcVec has comve from the _remote_ object.
*/
bool Msg::isDest() const
{
	return !( fv_->isDest() );
}

const Msg* Msg::next( const Element* e ) const
{
	if ( next_ == 0 )
		return 0;
	return e->msg( next_ );
}

unsigned int Msg::targets( vector< pair< Element*, unsigned int > >& list,
	unsigned int myEindex ) const
{
	vector< ConnTainer* >::const_iterator i;
	list.resize( 0 );
	for ( i = c_.begin(); i != c_.end(); i++ ) {
		for ( Conn* j = ( *i )->conn( myEindex, fv_->isDest() ); j->good(); j++ ) {
			pair< Element*, unsigned int > temp( 
				j->targetElement(), j->targetEindex() );
			list.push_back( temp );
		}
	}
	return list.size();
}

unsigned int Msg::numTargets( const Element* e ) const
{
	vector< ConnTainer* >::const_iterator i;
	unsigned int ret = 0;
	for ( i = c_.begin(); i != c_.end(); i++ ) {
		ret += ( *i )->size();
	}
	if ( next_ )
		ret += e->msg( next_ )->numTargets( e );

	return ret;
}

bool Msg::copy( const ConnTainer* c, Element* e1, Element* e2 ) const
{
	e1->checkMsgAlloc( c->msg1() );
	e2->checkMsgAlloc( c->msg2() );

	// Copying over the other parts of the Msg.
	// Problem will come when we copy a Msg with multiple target types,
	// hence the need to traverse 'next'.
	e1->varMsg( c->msg1() )->fv_ = fv_;
	e2->varMsg( c->msg2() )->fv_ = c->e2()->msg( c->msg2() )->fv_;
	return c->copy( e1, e2 );
}
