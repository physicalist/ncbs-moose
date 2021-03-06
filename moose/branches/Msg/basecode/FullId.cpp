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
//////////////////////////////////////////////////////////////
//	FullId I/O 
//////////////////////////////////////////////////////////////

ostream& operator <<( ostream& s, const FullId& i )
{
	if ( i.dataId.data() == 0 && i.dataId.field() == 0 )
		s << i.id;
	else 
		s << i.id << "[" << i.dataId << "]";
	return s;
}

/**
 * need to complete implementation
 */
istream& operator >>( istream& s, FullId& i )
{
	s >> i.id;
	return s;
}

Eref FullId::eref() const
{
	return Eref( id(), dataId );
}

bool FullId::operator==( const FullId& other ) const
{
	return ( id == other.id && dataId == other.dataId );
}

const FullId& FullId::bad()
{
	static FullId bad_( Id(), DataId::bad() );
	return bad_;
}
