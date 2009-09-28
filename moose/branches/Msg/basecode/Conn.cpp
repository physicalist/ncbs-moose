/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"

void Conn::asend( 
	const Element* e, FuncId f, const char* arg, unsigned int size ) const
{
	for( vector< Msg* >::const_iterator i = m_.begin(); i != m_.end(); ++i )
		(*i)->addToQ( e, f, arg, size );
}

/**
 * ClearQ calls clearQ on all Msgs, on e1.
 */
void Conn::clearQ()
{
	for( vector< Msg* >::const_iterator i = m_.begin(); i != m_.end(); ++i )
		(*i)->clearQ();
}

/**
* Add a msg to the list
*/
void Conn::add( Msg* m )
{
	m_.push_back( m );
}

/**
* Drop a msg from the list
*/
void Conn::drop( Msg* m )
{
	// Here we have the spectacularly ugly C++ erase-remove idiot.
	m_.erase( remove( m_.begin(), m_.end(), m ), m_.end() ); 
}
