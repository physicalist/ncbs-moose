/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2008 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"

const unsigned int MAX_SLOTS = 100;

vector< SimpleConnInfo >& setupSimpleConnInfo()
{
	static vector< SimpleConnInfo > sci( MAX_SLOTS );
	for ( unsigned int i = 0; i < MAX_SLOTS; i++ )
		sci[i] = SimpleConnInfo( i );

	return sci;
}

SimpleConnInfo* getSimpleConnInfo( unsigned int slotIndex )
{
	static vector< SimpleConnInfo >& sci = setupSimpleConnInfo();

	assert( slotIndex < MAX_SLOTS );
	return &( sci[ slotIndex ] );
}
