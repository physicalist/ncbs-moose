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
//	ObjId I/O 
//////////////////////////////////////////////////////////////

ostream& operator <<( ostream& s, const ObjId& i )
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
istream& operator >>( istream& s, ObjId& i )
{
	s >> i.id;
	return s;
}

Eref ObjId::eref() const
{
	return Eref( id(), dataId );
}

bool ObjId::operator==( const ObjId& other ) const
{
	return ( id == other.id && dataId == other.dataId );
}

const ObjId& ObjId::bad()
{
	static ObjId bad_( Id(), DataId::bad() );
	return bad_;
}

bool ObjId::isDataHere() const
{
	return id()->dataHandler()->isDataHere( dataId );
}

char* ObjId::data() const
{
	return id()->dataHandler()->data( dataId );
}

Element* ObjId::element() const
{
	return id();
}
