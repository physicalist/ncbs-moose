/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _OBJ_ID_H
#define _OBJ_ID_H

#ifdef USE_CHARMPP
#include "pup.h"
#include "CcsObjId.h"
#endif

#include <iostream>
using namespace std;

/**
 * This class manages lookups for specific data entries in elements,
 * in a node-independent manner.
 * It is basically a composite of Id and DataId.
 */
class ObjId
{
	friend ostream& operator <<( ostream& s, const ObjId& i );
	friend istream& operator >>( istream& s, ObjId& i );
	public:
		//////////////////////////////////////////////////////////////
		//	ObjId creation
		//////////////////////////////////////////////////////////////
		/**
		 * Returns the root Id
		 */
		ObjId()
			: id(), dataId()
		{;}

		/**
		 * Creates a ObjId using specified Id and DataId
		 */
		ObjId( Id i, DataId d )
			: id( i ), dataId( d )
		{;}

		ObjId( Id i )
			: id( i ), dataId( 0 )
		{;}

		/**
		 * Creates an ObjId by reading the path string
		 * Returns bad on failure.
		 */
		ObjId( const string& path );
#ifdef USE_CHARMPP
                ObjId(const CcsObjId &ccs) : 
                  id(ccs.id),
                  dataId(ccs.dataId)
                {}
#endif

		/**
		 * Returns the absolute path including all array indices.
		 */
		string path() const;

		/**
		 * Returns the Eref matching itself.
		 */
		Eref eref() const;

		/**
		 * For equality check
		 */
		bool operator==( const ObjId& other ) const;

		/**
		 * True if the data is present on the current node.
		 */
		bool isDataHere() const;

		/**
		 * Returns data entry for this object
		 */
		char* data() const;

		/**
		 * Returns Element part
		 */
		Element* element() const;

		/**
		 * Here are the data values.
		 */
		Id id; 
		DataId dataId;

		/// A useful test value
		static const ObjId bad();
};

#endif // _OBJ_ID_H
