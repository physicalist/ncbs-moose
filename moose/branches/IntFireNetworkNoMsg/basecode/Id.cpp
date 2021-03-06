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
//	Id creation
//////////////////////////////////////////////////////////////

Id::Id()
	: id_( 0 ), index_( 0 )
{;}

Id::Id( const string& path, const string& separator )
{
	;
}

/**
 * Static func to extract an id from a string. We need to accept ids
 * out of the existing range, but it would be nice to have a heuristic
 * on how outrageous the incoming value is.
 */
Id Id::str2Id( const std::string& s )
{
	// unsigned int val = atoi( s.c_str() );
	return Id( s );
}

//////////////////////////////////////////////////////////////
//	Element array static access function. Private.
//////////////////////////////////////////////////////////////

vector< Element* >& Id::elements()
{
	static vector< Element* > e;
	return e;
}

//////////////////////////////////////////////////////////////
//	Id info
//////////////////////////////////////////////////////////////

// static func to convert id into a string.
string Id::id2str( Id id )
{
	return id.path();
}

// Function to convert it into its fully separated path.
string Id::path( const string& separator) const 
{
	return "";
}

/**
 * Here we work with a single big array of all ids. Off-node elements
 * are represented by their postmasters. When we hit a postmaster we
 * put the id into a special field on it. Note that this is horrendously
 * thread-unsafe.
 * \todo: I need to replace the off-node case with a wrapper Element
 * return. The object stored here will continue to be the postmaster,
 * and when this is detected it will put the postmaster ptr and the id
 * into the wrapper element. The wrapper's own id will be zero so it
 * can be safely deleted.
 */
Element* Id::operator()() const
{
	return elements()[ id_ ];
}

unsigned int Id::index() const 
{
	return index_;
}

Eref Id::eref() const 
{
	return Eref( elements()[ id_ ], index_ );
}

Id Id::create( Element* e )
{
	elements().push_back( e );
	return Id( elements().size() - 1, 0 );
}

//////////////////////////////////////////////////////////////
//	Id utility
//////////////////////////////////////////////////////////////

ostream& operator <<( ostream& s, const Id& i )
{
	if ( i.index_ == 0 )
		s << i.id_;
	else 
		s << i.id_ << "[" << i.index_ << "]";
	return s;
}

istream& operator >>( istream& s, Id& i )
{
	s >> i.id_;
	return s;
}

Id::Id( unsigned int id, unsigned int index )
	: id_( id ), index_( index )
{
	;
}
