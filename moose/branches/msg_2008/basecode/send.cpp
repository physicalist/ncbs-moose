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
#include "MsgSrc.h"
#include "MsgDest.h"
#include "SimpleElement.h"
#include "send.h"

void send0( const Element* eIn, Slot src )
{
	const SimpleElement* e = static_cast< const SimpleElement* >( eIn );

	do {
		RecvFunc rf = e->srcRecvFunc( src.msg() );
		/*
		for_each ( e->connSrcBegin( src.msg() ), 
			e->connSrcEnd( src.msg() ), rf );
			*/
		vector< Conn >::const_iterator j;
		for ( j = e->connSrcBegin( src.msg() );
				j != e->connSrcEnd( src.msg() ); j++ )
			rf( &( *j ) );
		src = Slot( e->nextSrc( src.msg() ), src.func() );
	} while ( src.msg() != 0 );
}

void sendTo0( const Element* eIn, Slot src, unsigned int conn )
{
	const SimpleElement* e = static_cast< const SimpleElement* >( eIn );
	RecvFunc rf = e->lookupRecvFunc( src.msg(), conn );
		rf( &( *( e->lookupConn( conn ) ) ) );
}
