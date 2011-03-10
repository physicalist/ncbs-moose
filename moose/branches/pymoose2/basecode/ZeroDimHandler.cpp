/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"

#include "ReduceBase.h"
#include "ReduceMax.h"
#include "../shell/Shell.h"

ZeroDimHandler::ZeroDimHandler( const DinfoBase* dinfo )
	: ZeroDimGlobalHandler( dinfo )
{;}

ZeroDimHandler::ZeroDimHandler( const ZeroDimHandler* other )
	: ZeroDimGlobalHandler( other->dinfo() )
{
	if ( Shell::myNode() == 0 )
		data_ = dinfo()->copyData( other->data_, 1, 1 );
	else
		data_ = 0;
}

ZeroDimHandler::~ZeroDimHandler()
{
	// dinfo()->destroyData( data_ );
}

DataHandler* ZeroDimHandler::globalize() const
{
	return 0;
}

DataHandler* ZeroDimHandler::unGlobalize() const
{
	return new ZeroDimHandler( this );
}

bool ZeroDimHandler::nodeBalance( unsigned int size )
{
	return 0;
}

DataHandler* ZeroDimHandler::copy() const
{
	return ( new ZeroDimHandler( this ) );
}

DataHandler* ZeroDimHandler::copyExpand( unsigned int copySize ) const
{
	OneDimHandler* ret = new OneDimHandler( dinfo() );
	vector< unsigned int > dims( 1, copySize );
	ret->resize( dims );
	for ( iterator i = ret->begin(); i != ret->end(); ++i ) {
		char* temp = *i;
		memcpy( temp, data_, dinfo()->size() );
	}
	return ret;
}

DataHandler* ZeroDimHandler::copyToNewDim( unsigned int newDimSize ) const
{
	return copyExpand( newDimSize );
}


void ZeroDimHandler::process( const ProcInfo* p, Element* e, FuncId fid ) const
{
	if ( Shell::myNode() == 0 && 
		p->threadIndexInGroup == p->numThreadsInGroup - 1 ) {
		// reinterpret_cast< Data* >( data_ )->process( p, Eref( e, 0 ) );

		const OpFunc* f = e->cinfo()->getOpFunc( fid );
		const ProcOpFuncBase* pf = dynamic_cast< const ProcOpFuncBase* >( f );
		assert( pf );
		pf->proc( data_, Eref( e, 0 ), p );
	}
}

unsigned int ZeroDimHandler::localEntries() const {
	return ( Shell::myNode() == 0 );
}


bool ZeroDimHandler::resize( vector< unsigned int > dims )
{
	if ( Shell::myNode() == 0 && !data_ ) {
		data_ = dinfo()->allocData( 1 );
	}
	return 1;
}

char* ZeroDimHandler::data( DataId index ) const {
	return data_;
}

/**
 * Returns true if the node decomposition has the data on the
 * current node
 */
bool ZeroDimHandler::isDataHere( DataId index ) const {
	return ( Shell::myNode() == 0 );
}

bool ZeroDimHandler::isAllocated() const {
	return data_ != 0;
}

DataHandler::iterator ZeroDimHandler::end() const
{
	// cout << Shell::myNode() << ": ZeroDimHandler Iterator\n";
	// end is same as begin except on node 1.
	unsigned int i = ( Shell::myNode() == 0 );
	return iterator( this, i, i );
}

bool ZeroDimHandler::setDataBlock( 
	const char* data, unsigned int numData, 
	const vector< unsigned int >& startIndex ) const
{ 
	if ( startIndex.size() != 0 ) return 0;
	return setDataBlock( data, numData, 0 );
}

bool ZeroDimHandler::setDataBlock( 
	const char* data, unsigned int numData, 
	DataId startIndex ) const
{ 
	if ( numData != 1 ) return 0;
	if ( startIndex.data() != 0 ) return 0;
	if ( !isAllocated() ) return 0;
	if ( Shell::myNode() == 0 ) {
		memcpy( data_, data, dinfo()->size() );
	}
	return 1;
}
