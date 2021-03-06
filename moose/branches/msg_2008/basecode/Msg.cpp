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

bool Msg::assignMsgByFuncId( 
	Element* e, unsigned int funcId, ConnTainer* ct )
{
	if ( fv_->id() == 0 )
		fv_ = FuncVec::getFuncVec( funcId );

	if ( fv_->id() == funcId ) {
		c_.push_back( ct );
		return 1;
	}

	if ( next_ ) {
		assert( next_ < e->numMsg() );
		Msg *m = e->varMsg( next_ );
		if ( !m )
			return 0;
		return ( m->assignMsgByFuncId( e, funcId, ct ) );
	}
	
	// No matching slot, so make a new one.
	// Note that the addNextMsg function may invalidate 'this'
	// because it resizes the Element::msg_ vector. So don't access
	// any Msg fields after this point.
	unsigned int temp = next_ = e->numMsg();
	unsigned int temp2 = e->addNextMsg();
	assert( temp == temp2 );
	Msg *m = e->varMsg( temp );
	if ( !m )
		return 0;

	m->fv_ = FuncVec::getFuncVec( funcId );
	m->c_.push_back( ct );
	m->next_ = 0;
	return 1;
}

/**
 * Add a new message using the specified ConnTainer.
 * The e1 (source ) and e2 (dest), are in the ConnTainer, as are
 * m1 and m2 which indicate source and dest msg indices.
 * The funcId1 is the source funcId, which is going to be used
 * at the dest, but is optional so it may be zero.
 * The funcId2 is the dest funcId, which must be nonzero and will
 * be used when the source calls the dest.
 * Later I may relax the directional restrictions.
 *
 * Returns true on success.
 */
bool Msg::add( ConnTainer* ct,
	unsigned int funcId1, unsigned int funcId2 )
{
	// Must always have a nonzero func on the destination
	assert( funcId2 != 0 );

	Msg* m1 = ct->e1()->varMsg( ct->msg1() );
	if ( !m1 ) return 0;

	// We need to check for msg2 because DynamicFinfos have a +ve
	// msg, even if they are handling pure dests.
	if ( funcId1 == 0 && ct->msg2() < 0 ) { // a destOnly msg, terminating in destMsg_.
		// Look for, and if necessary create the connTainer.
		vector< ConnTainer* >* dct = ct->e2()->getDest( ct->msg2() ); 
		assert( dct != 0 );
		bool ret = m1->assignMsgByFuncId( ct->e1(), funcId2, ct );
		assert( ret );
		dct->push_back( ct );
	} else { // A msg terminating in the msg_ vector
		Msg* m2 = ct->e2()->varMsg( ct->msg2() );
		if ( !m2 ) return 0;
		bool ret;
		ret = m1->assignMsgByFuncId( ct->e1(), funcId2, ct );
		assert( ret );
		ret = m2->assignMsgByFuncId( ct->e2(), funcId1, ct );
		assert( ret );
	}
	return 1;
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
 * innerDrop eliminates an identified ConnTainer.
 * This is NOT the call to initiate removing a connection.
 * It is called on the other end of the message from the one directly
 * set up for deletion, and assumes that the rest of the message will
 * be taken care of by the initiating function.
 * Assumes that the element checks first to see if it is also doomed.
 * If the element is to survive, only then it goes through the bother
 * of erasing.
 *
 * Note that this does not do garbage collection if a 'next' Msg is 
 * emptied. Something to think about, much later.
 */
bool Msg::innerDrop( const ConnTainer* doomed )
{
	vector< ConnTainer* >::iterator pos = 
		find( c_.begin(), c_.end(), doomed );
	if ( pos != c_.end() ) {
		c_.erase( pos );
		return 1;
	} else if ( next_ != 0 ) {
		if ( fv_->isDest() ) // The current msg is source.
			return doomed->e1()->varMsg( next_ )->innerDrop( doomed );
		else
			return doomed->e2()->varMsg( next_ )->innerDrop( doomed );
	}
	cout << "Msg::drop( const ConnTainer* doomed ): can't find doomed\n"; 
	return 0;
}

/**
 * Utility function for dropping target, whether it is 
 * on a pure dest or another Msg.
 * Does not clear the ConnTainer.
 * NOT the primary call to drop a message.
 */
bool Msg::innerDrop( Element* remoteElm, int remoteMsgNum,
	const ConnTainer* d )
{
	assert( remoteElm != 0 );

	if ( remoteMsgNum < 0 ) { // A pure dest msg.
		vector< ConnTainer* >* ct = remoteElm->getDest( remoteMsgNum );
		vector< ConnTainer* >::iterator i;
		assert ( ct->size() > 0 );
		// Find the doomed ConnTainer and shift it to the end of the
		// vector.
		i = remove( ct->begin(), ct->end(), d );
		if ( i == ct->end() ) // It had better exist.
			return 0;
		ct->erase( i ); // zap it on dest
		return 1;
	} else {
		Msg* remoteMsg = remoteElm->varMsg( remoteMsgNum );
		assert( remoteMsg != 0 );
		return remoteMsg->innerDrop( d );
	}
}

/**
 * This variant of drop initiates the removal of a specific ConnTainer,
 * identified by its index within the Msg. 
 * Clearly, this cannot be called from a pure dest.
 */
bool Msg::drop( Element* e, unsigned int doomed )
{
	if ( doomed < c_.size() ) {
		ConnTainer* d = c_[ doomed ];
		int remoteMsgNum;
		Element* remoteElm;
		if ( fv_->isDest() ) {
			remoteMsgNum = d->msg2();
			remoteElm = d->e2();
		} else {
			remoteMsgNum = d->msg1();
			remoteElm = d->e1();
		}
		if ( innerDrop( remoteElm, remoteMsgNum, d ) ) {
			c_.erase( c_.begin() + doomed );
			delete d;
			return 1;
		} else {
			return 0;
		}
	} else { // Presumably it is on a next_ msg.
		assert ( next_ < e->numMsg() );
		if ( next_ )
			return e->varMsg( next_ )->drop( e, doomed - c_.size() );
	}
	// No, the conn doesn't exist at all.
	cout << "Msg::drop( unsigned int doomed ): doomed outside range\n"; 
	return 0;
}

/**
 * This variant of drop initiates the removal of a specific ConnTainer,
 * identified by itself.
 * Clearly, this cannot be called from a pure dest.
 */
bool Msg::drop( Element* e, const ConnTainer* doomed )
{
	vector< ConnTainer * >::iterator pos;
	pos = find( c_.begin(), c_.end(), doomed );
	if ( pos != c_.end() ) {
		int remoteMsgNum;
		Element* remoteElm;
		if ( fv_->isDest() ) {
			remoteMsgNum = doomed->msg2();
			remoteElm = doomed->e2();
		} else {
			remoteMsgNum = doomed->msg1();
			remoteElm = doomed->e1();
		}
		if ( innerDrop( remoteElm, remoteMsgNum, doomed ) ) {
			c_.erase( pos );
			delete doomed;
			return 1;
		} else {
			return 0;
		}
	} else { // Presumably it is on a next_ msg.
		assert ( next_ < e->numMsg() );
		if ( next_ )
			return e->varMsg( next_ )->drop( e, doomed );
	}
	// No, the conn doesn't exist at all.
	cout << "Msg::drop( ConnTainer* doomed ): doomed not on Msg.\n"; 
	return 0;
}

/**
 * dropAll cleans up the entire set of ConnTainers on this msg.
 */
void Msg::dropAll( Element* e )
{
	vector< ConnTainer* >::iterator i;

	for ( i = c_.begin(); i != c_.end(); i++ ) {
		int remoteMsgNum;
		Element* remoteElm;
		if ( fv_->isDest() ) {
			remoteMsgNum = ( *i )->msg2();
			remoteElm = ( *i )->e2();
		} else {
			remoteMsgNum = ( *i )->msg1();
			remoteElm = ( *i )->e1();
		}
		bool ret = innerDrop( remoteElm, remoteMsgNum, *i );
		if ( ret )
			delete( *i );
		else
			cout << "Error: Msg::dropAll(): remoteMsg failed to drop\n"; 
	}
	assert ( next_ < e->numMsg() );
	if ( next_ )
		e->varMsg( next_ )->dropAll( e );
	next_ = 0; // Note that we don't yet do garbage collection to
				// reuse the 'next' msg location on the vector.
	c_.resize( 0 );
}

/**
 * Deletes all the messages going outside the current tree from the current
 * Msg. Used when
 * dropAll cleans up all messages to targets outside tree being deleted.
 * The objects within the tree have their 'isMarkedForDeletion' flag set.
 * We don't need to worry about 'next' here because it is called 
 * sequentially for every single entry in the msg_ vector
 */
void Msg::dropRemote()
{
	vector< ConnTainer* >::iterator i;

	for ( i = c_.begin(); i != c_.end(); i++ ) {
		int remoteMsgNum;
		Element* remoteElm;
		if ( fv_->isDest() ) {
			remoteMsgNum = ( *i )->msg2();
			remoteElm = ( *i )->e2();
		} else {
			remoteMsgNum = ( *i )->msg1();
			remoteElm = ( *i )->e1();
		}
		if ( !remoteElm->isMarkedForDeletion() ) {
			bool ret = innerDrop( remoteElm, remoteMsgNum, *i );
			if ( ret )
				delete( *i );
			else
				cout << "Error: Msg::dropRemote(): remoteMsg failed to drop\n";
			*i = 0;
		}
	}
	// STL magic here. Gag all you want.
	// The idea is to first shuffle all the zero c_ entries to the end.
	i = remove_if( c_.begin(), c_.end(), bind2nd( equal_to< ConnTainer* >(), 0 ) );
	// Then we get rid of them.
	c_.erase( i, c_.end() );
}

/**
 * Deletes all the messages originating from outside the current tree.
 * This is called from the viewpoint of the destination ConnTainer
 * on an Element to be deleted.
 * A static function, nothing much to do with the Msg class.
 * Here only because it keeps all the related deletion
 * operations in one place.
 */
void Msg::dropDestRemote( vector< ConnTainer* >& ctv  )
{
	vector< ConnTainer* >::iterator k;
	for ( k = ctv.begin(); k != ctv.end(); k++ ) {
		if ( !( *k )->e1()->isMarkedForDeletion() ) {
			bool ret = Msg::innerDrop( ( *k )->e1(),
				( *k )->msg1(), *k );
			if ( ret )
				delete ( *k );
			else
				cout << "Error: SimpleElement::prepareForDeletion(): remoteMsg failed to drop\n";
			*k = 0;
		}
	}
	// Note that this cleanup is not strictly necessary, as the
	// eventual deletion will remove all these too. But it is
	// cleaner to be done here than leave it to later.
	k = remove_if ( ctv.begin(), ctv.end(), bind2nd( equal_to< ConnTainer* >(), 0 ) );
	ctv.erase( k, ctv.end() );
}

/**
 * Drops all messages during deletion. Assumes that all targets
 * are also scheduled for deletion.
 */
void Msg::dropForDeletion()
{
	vector< ConnTainer* >::iterator i;

	for ( i = c_.begin(); i != c_.end(); i++ ) {
		if ( fv_->isDest() ) { // The current msg is source.
			/*
			 * Can't refer to remote object: it may not exist any more.
			remoteMsg = ( *i )->e2()->varMsg( ( *i )->msg2() );
			assert( remoteMsg != 0 );
			assert( remoteMsg != this );
			*/
			delete( *i );
		}
	}
	c_.resize( 0 );
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
			return ( *i )->conn( eIndex, !( fv_->isDest() ), tgt );
		}
	}
	return 0;
}

/**
* True if this is the nominal destination of a message.
* Undefined if the message is empty: Check for size first.
* The definition of message source and dest is done at Finfo
* setup time. For simple messages no problem. For Shared Finfos,
* the one that has the first 'source' entry is the source.
*
* Note that the value is the complement of funcVec flag, because
* the funcVec has comve from the _remote_ object.
*/
bool Msg::isDest() const
{
	assert( c_.size() > 0 );
	return !( fv_->isDest() );
}

const Msg* Msg::next( const Element* e ) const
{
	if ( next_ == 0 )
		return 0;
	assert( next_ < e->numMsg() );
	return e->msg( next_ );
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
	unsigned int funcId1 = 0; // True if it was a pure Dest.
	if ( c->msg2() >= 0 ) {
		const Msg* m2 = c->e2()->msg( c->msg2() );
		funcId1 = m2->fv_->id();
	}
	unsigned int funcId2 = fv_->id(); // from e2, stored on m1.
	ConnTainer* ct = c->copy( e1, e2 );
	if ( ct == 0 )
		return 0;
	return add( ct, funcId1, funcId2 );
}
