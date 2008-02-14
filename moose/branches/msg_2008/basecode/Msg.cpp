/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
**           copyright (C) 2003-2008 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "Msg.h"

Msg::Msg()
	isDest_ = 0;
{;}

Msg::~Msg()
{
	dropAll();
}

ConnTainer* Msg::add( Element* e1, Element* e2, 
	unsigned int m1Index, unsigned int m2Index )
{
	Msg* current = e1->msg( m1Index );
	if ( !current ) return 0;

	Msg* other = e2->msg( m2Index );
	if ( !other ) return 0;

	SimpleConnTainer* ct = new SimpleConnTainer( e1, e2, m1Index, m2Index );
	current->c_.push_back( ct );
	other->c_.push_back( ct );
	return ct;
}


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
		c_.erase( pos );
		return 1;
	}
	cout << "Msg::drop( const ConnTainer* doomed ): can't find doomed\n"; 
	return 0
}

/**
 * This variant of drop initiates the removal of a specific ConnTainer,
 * identified by its index within the Msg. 
 * Note... doesn't yet handle next Msg.
 */
bool Msg::drop( unsigned int doomed )
{
	if ( doomed < c_.size() ) {
		ConnTainer* d = &( c_[ doomed ] );
		if ( isDest ) {
			if ( d->e1()->dropConnTainer( d, d->msg1() ) ) {
				c_.erase( c_.begin() + doomed );
				delete d;
				return 1;
			}
		} else {
			if ( d->e2()->dropConnTainer( d, d->msg2() ) ) {
				c_.erase( c_.begin() + doomed );
				delete d;
				return 1;
			}
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
	if ( isDest ) {
		for ( i = c_.begin(); i != c_.end(); i++ ) {
			( *i )->e1()->dropConnTainer( *i, (*i)->msg1() );
			delete( *i );
		}
	} else {
		for ( i = c_.begin(); i != c_.end(); i++ ) {
			( *i )->e2()->dropConnTainer( *i, (*i)->msg2() );
			delete( *i );
		}
	}
}

unsigned int Msg::size() const
{
	return c_.size();
}

unsigned int Msg::targets( vector< pair< Element*, unsigned int > >& list,
	myEindex ) const
{
	vector< ConnTainer* >::const_iterator i;
	list.resize( 0 );
	for ( i = c_.begin(); i != c_.end(); i++ ) {
		for ( Conn* j = i->conn( myEindex, isDest() ); j->good(); j++ ) {
			pair< Element*, unsigned int > temp( 
				j->targetElement(), j->targetEindex() );
			list.push_back( temp );
		}
	}
	return list.size();
}

bool Msg::copy( const ConnTainer* c, Element* e1, Element* e2 ) const
{
	e1->checkMsgAlloc( c->msg1() );
	e2->checkMsgAlloc( c->msg2() );

	// Copying over the other parts of the Msg.
	// Problem will come when we copy a Msg with multiple target types,
	// hence the need to traverse 'next'.
	e1->varMsg( c->msg1() )->fv_ = fv_;
	e1->varMsg( c->msg1() )->isDest_ = isDest_;

	e2->varMsg( c->msg2() )->fv_ = c->e2()->Msg( c->msg2() )->fv_;
	e2->varMsg( c->msg2() )->isDest_ = ~isDest_;

	c->copy( e1, e2 );
}
