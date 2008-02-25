/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include <algorithm>
#include "SimpleElement.h"
#include "Send.h"

void send0( const Element* e, unsigned int eIndex, Slot src )
{
	const Msg* m = e->msg( src.msg() );
	do {
		RecvFunc rf = m->func( src.func() );
		vector< ConnTainer* >::const_iterator i;
		// Going through the MsgSrc vector of ConnTainers
		for ( i = m->begin( ); i != m->end( ); i++ ) {
			for ( Conn* j = ( *i )->conn( eIndex, m->isDest() ); 
				j->good(); j->increment() )
				rf( j );
		}
	} while ( ( m = m->next( e ) ) ); // An assignment, not a comparison.
}

void sendTo0( const Element* e, unsigned int eIndex, Slot src,
	unsigned int tgt )
{
	// This will traverse through next() if needed, to get to the msg.
	const Msg* m = e->msg( src.msg() ); 
	RecvFunc rf = m->func( src.func() );
	Conn* j = m->findConn( eIndex, tgt );
	rf( j );
}

void sendBack0( const Element* e, Slot src, const Conn* c )
{
	const Msg* m = e->msg( src.msg() );
	RecvFunc rf = m->func( src.func() );
	const Conn* flip = c->flip(); /// \todo Could be optimized.
	rf( flip );
	delete flip;
}
