/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SEND_H
#define _SEND_H
#include <vector>
#include "Element.h"
#include "SimpleElement.h"
#include "Conn.h"

//////////////////////////////////////////////////////////////////////////
//                      Zero argument section
//////////////////////////////////////////////////////////////////////////
/**
 * This function sends zero-argument messages.
 */
extern void send0( const Element* e, unsigned int eIndex, Slot src );

/**
 * This function sends zero-argument messages to a specific target.
 * The index is the index in the local conn_ vector of the source Msg.
 */
extern void sendTo0( const Element* e, unsigned int eIndex, Slot src, 
				unsigned int index);

/**
 * sendBack returns zero-argument messages to sender. Index info is
 * already there in the Conn.
 */
void sendBack0( const Element* e, Slot src, const Conn* c );

//////////////////////////////////////////////////////////////////////////
//                      One argument section
//////////////////////////////////////////////////////////////////////////

/**
 * This templated function sends single-argument messages.
 */
template < class T > void send1(
		const Element* e, unsigned int eIndex, Slot src, T val )
{
	const Msg* m = e->msg( src.msg() );
	do {
		void( *rf )( const Conn*, T ) = 
			reinterpret_cast< void ( * )( const Conn*, T ) >(
				m->func( src.func() )
			);
		vector< ConnTainer* >::const_iterator i;
		for ( i = m->begin(); i != m->end(); i++ ) {
			for ( Conn* j = ( *i )->conn( eIndex, m->isDest() );
				j->good(); j++ )
				rf( j, val );
		}
	// Yes, it is an assignment, not a comparison
	} while ( ( m = m->next() ) ); 
}

/**
 * This templated function sends a single-argument message to the
 * target specified by the conn argument. Note that this refers
 * to the index in the local conn_ vector.
 */
template< class T > void sendTo1(
		const Element* e, unsigned int eIndex,
		Slot src, unsigned int tgt, T val )
{
	const Msg* m = e->msg( src.msg() );
	void( *rf )( const Conn*, T ) = 
			reinterpret_cast< void ( * )( const Conn*, T ) >(
			m->func( src.func() )
		);
	const Conn* j = m->findConn( eIndex, tgt );
	rf( j,  val );
}

template< class T > void sendBack1( const Element* e, Slot src, 
	const Conn* c, T val )
{
	const Msg* m = e->msg( src.msg() );
	void( *rf )( const Conn*, T ) = 
			reinterpret_cast< void ( * )( const Conn*, T ) >(
			m->func( src.func() )
		);
	const Conn* flip = c->flip();
	rf( flip, val );
	delete flip;
}

//////////////////////////////////////////////////////////////////////////
//                      Two argument section
//////////////////////////////////////////////////////////////////////////
/**
 * This templated function sends two-argument messages.
 */
template < class T1, class T2 > void send2(
		const Element* e, unsigned int eIndex, Slot src, T1 v1, T2 v2 )
{
	const Msg* m = e->msg( src.msg() );
	do {
		void( *rf )( const Conn*, T1, T2 ) = 
			reinterpret_cast< void ( * )( const Conn*, T1, T2 ) >(
				m->func( src.func() )
			);
		vector< ConnTainer* >::const_iterator i;
		for ( i = m->begin(); i != m->end(); i++ ) {
			for ( Conn* j = ( *i )->conn( eIndex, m->isDest() );
				j->good(); j++ )
				rf( j, v1, v2 );
		}
	// Yes, it is an assignment, not a comparison
	} while ( ( m = m->next() ) ); 
}

/**
 * This templated function sends a single-argument message to the
 * target specified by the conn argument. Note that this refers
 * to the index in the local conn_ vector.
 */
template< class T1, class T2 > void sendTo2(
		const Element* e, unsigned int eIndex,
		Slot src, unsigned int tgt, T1 v1, T2 v2 )
{
	const Msg* m = e->msg( src.msg() );
	void( *rf )( const Conn*, T1, T2 ) = 
			reinterpret_cast< void ( * )( const Conn*, T1, T2 ) >(
			m->func( src.func() )
		);
	const Conn* j = m->findConn( eIndex, tgt );
	rf( j,  v1, v2 );
}

template< class T1, class T2 > void sendBack2( const Element* e, Slot src, 
	const Conn* c, T1 v1, T2 v2 )
{
	const Msg* m = e->msg( src.msg() );
	void( *rf )( const Conn*, T1, T2 ) = 
			reinterpret_cast< void ( * )( const Conn*, T1, T2 ) >(
			m->func( src.func() )
		);
	Conn* flip = c->flip();
	rf( flip, v1, v2 );
	delete flip;
}

//////////////////////////////////////////////////////////////////////////
//                      Three argument section
//////////////////////////////////////////////////////////////////////////

/**
 * This templated function sends three-argument messages.
 */
template < class T1, class T2, class T3 > void send3(
	const Element* e, unsigned int eIndex, Slot src, T1 v1, T2 v2, T3 v3 )
{
	const Msg* m = e->msg( src.msg() );
	do {
		void( *rf )( const Conn*, T1, T2, T3 ) = 
			reinterpret_cast< void ( * )( const Conn*, T1, T2, T3 ) >(
				m->func( src.func() )
			);
		vector< ConnTainer* >::const_iterator i;
		for ( i = m->begin(); i != m->end(); i++ ) {
			for ( Conn* j = ( *i )->conn( eIndex, m->isDest() );
				j->good(); j++ )
				rf( j, v1, v2, v3 );
		}
	// Yes, it is an assignment, not a comparison
	} while ( ( m = m->next() ) ); 
}

/**
 * This templated function sends a single-argument message to the
 * target specified by the conn argument. Note that this refers
 * to the index in the local conn_ vector.
 */
template< class T1, class T2, class T3 > void sendTo3(
		const Element* e, unsigned int eIndex,
		Slot src, unsigned int tgt, T1 v1, T2 v2, T3 v3 )
{
	const Msg* m = e->msg( src.msg() );
	void( *rf )( const Conn*, T1, T2, T3 ) = 
			reinterpret_cast< void ( * )( const Conn*, T1, T2, T3 ) >(
			m->func( src.func() )
		);
	const Conn* j = m->findConn( eIndex, tgt );
	rf( j,  v1, v2, v3 );
}

template< class T1, class T2, class T3 > 
	void sendBack3( const Element* e, Slot src, 
	const Conn* c, T1 v1, T2 v2, T3 v3 )
{
	const Msg* m = e->msg( src.msg() );
	void( *rf )( const Conn*, T1, T2, T3 ) = 
			reinterpret_cast< void ( * )( const Conn*, T1, T2, T3 ) >(
			m->func( src.func() )
		);
	Conn* flip = c->flip();
	rf( flip, v1, v2, v3 );
	delete flip;
}

//////////////////////////////////////////////////////////////////////////
//                      Four argument section
//////////////////////////////////////////////////////////////////////////
/**
 * This templated function sends four-argument messages.
 */
template < class T1, class T2, class T3, class T4 > void send4(
	const Element* e, unsigned int eIndex, Slot src, 
	T1 v1, T2 v2, T3 v3, T4 v4 )
{
	const Msg* m = e->msg( src.msg() );
	do {
		void( *rf )( const Conn*, T1, T2, T3, T4 ) = 
			reinterpret_cast< void ( * )( const Conn*, T1, T2, T3, T4 ) >(
				m->func( src.func() )
			);
		vector< ConnTainer* >::const_iterator i;
		for ( i = m->begin(); i != m->end(); i++ ) {
			for ( Conn* j = ( *i )->conn( eIndex, m->isDest() );
				j->good(); j++ )
				rf( j, v1, v2, v3, v4 );
		}
	// Yes, it is an assignment, not a comparison
	} while ( ( m = m->next() ) );
}

/**
 * This templated function sends a single-argument message to the
 * target specified by the conn argument. Note that this refers
 * to the index in the local conn_ vector.
 */
template< class T1, class T2, class T3, class T4 > void sendTo4(
		const Element* e, unsigned int eIndex,
		Slot src, unsigned int tgt, T1 v1, T2 v2, T3 v3, T4 v4 )
{
	const Msg* m = e->msg( src.msg() );
	void( *rf )( const Conn*, T1, T2, T3, T4 ) = 
			reinterpret_cast< void ( * )( const Conn*, T1, T2, T3, T4 ) >(
			m->func( src.func() )
		);
	const Conn* j = m->findConn( eIndex, tgt );
	rf( j,  v1, v2, v3, v4 );
}

template< class T1, class T2, class T3, class T4 > 
	void sendBack4( const Element* e, Slot src, 
	const Conn* c, T1 v1, T2 v2, T3 v3, T4 v4 )
{
	const Msg* m = e->msg( src.msg() );
	void( *rf )( const Conn*, T1, T2, T3, T4 ) = 
			reinterpret_cast< void ( * )( const Conn*, T1, T2, T3, T4 ) >(
			m->func( src.func() )
		);
	Conn* flip = c->flip();
	rf( flip, v1, v2, v3, v4 );
	delete flip;
}

#endif // _SEND_H
