/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _DATAID_H
#define _DATAID_H

/**
 * Handles lookup of data fields in an Element. In most cases all we need
 * is the index for the data object. 
 * In principle, this object should be able to handle arbitrary
 * dimensions. Alternatively, it should be able to index more than 1e10
 * data entries. However, the total address space will be unlikely to exceed
 * 64 bits for a while, so one could imagine doing different kinds of
 * bitfields out of the total. For now, just provide unsigned int for each
 * of the data and field indices.
 */

class DataId {
	public:
		friend ostream& operator <<( ostream& s, const DataId& e );
		DataId()
			: data_( 0 ), field_( 0 )
		{;}

		DataId( unsigned int dIndex )
			: data_( dIndex ), field_( 0 )
		{;}

		DataId( unsigned int data, unsigned int field )
			: data_( data ), field_( field )
		{;}

		unsigned int data() const {
			return data_;
		}

		unsigned int field() const {
			return field_;
		}
	
	private:
		unsigned int data_;
		unsigned int field_;
};

#endif // _DATAID_H
