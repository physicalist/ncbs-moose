/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "AnyDimGlobalHandler.h"
#include "AnyDimHandler.h"

OneDimGlobalHandler::OneDimGlobalHandler( const DinfoBase* dinfo )
		: DataHandler( dinfo ), 
			data_( 0 ), size_( 0 )
{;}

OneDimGlobalHandler::OneDimGlobalHandler( const OneDimGlobalHandler* other )
		: DataHandler( other->dinfo() ), 
			size_( other->size_ )
{
	data_ = dinfo()->copyData( other->data_, size_, size_ );
}

OneDimGlobalHandler::~OneDimGlobalHandler() {
	dinfo()->destroyData( data_ );
}

DataHandler* OneDimGlobalHandler::globalize() const
{
	return copy();
}


DataHandler* OneDimGlobalHandler::unGlobalize() const
{
	OneDimHandler* ret = new OneDimHandler( dinfo() );
	vector< unsigned int > dims( 1, size_ );
	ret->resize( dims );
	ret->setDataBlock( data_, size_, 0 );
	return ret;
}

DataHandler* OneDimGlobalHandler::copy() const
{
	return ( new OneDimGlobalHandler( this ) );
}

DataHandler* OneDimGlobalHandler::copyExpand( unsigned int copySize ) const
{
	OneDimGlobalHandler* ret = new OneDimGlobalHandler( dinfo() );
	vector< unsigned int > dims( 1, copySize );
	ret->resize( dims );
	unsigned int s = size_;
	for ( unsigned int offset = 0; offset < copySize; offset += size_ ) {
		if ( s > ( copySize - offset ) )
			s = copySize - offset;
		memcpy( ret->data_ + offset * dinfo()->size(), data_, 
			s * dinfo()->size() );
	}
	return ret;
}

/**
 * Expand it into a 2-dimensional version of AnyDimGlobalHandler.
 */
DataHandler* OneDimGlobalHandler::copyToNewDim( unsigned int newDimSize ) 
	const
{
	AnyDimGlobalHandler* ret = new AnyDimGlobalHandler( dinfo() );
	vector< unsigned int > dims( 2 );
	dims[1] = newDimSize;
	dims[0] = size_;
	ret->resize( dims );

	for ( unsigned int i = 0; i < newDimSize; ++i ) {
		// setDataBlock( const char* data, unsigned int dataSize, unsigned int dimNum, unsigned int dimIndex )
		ret->setDataBlock( data_, size_, i * size_ );
	}
	return ret;
}

/**
 * Handles both the thread and node decomposition
 * Here there is no node decomposition: all entries are present
 * on all nodes.
 */
void OneDimGlobalHandler::process( const ProcInfo* p, Element* e, 
	FuncId fid ) const
{
	char* temp = data_ + p->threadIndexInGroup * dinfo()->size();
	unsigned int stride = dinfo()->size() * p->numThreadsInGroup;

	const OpFunc* f = e->cinfo()->getOpFunc( fid );
	const ProcOpFuncBase* pf = dynamic_cast< const ProcOpFuncBase* >( f );
	assert( pf );

	for ( unsigned int i = p->threadIndexInGroup; i < size_; 
		i+= p->numThreadsInGroup )
	{
		// reinterpret_cast< Data* >( temp )->process( p, Eref( e, i ) );
		pf->proc( temp, Eref( e, i ), p );
		temp += stride;
	}
}

unsigned int OneDimGlobalHandler::totalEntries() const {
	return size_;
}

unsigned int OneDimGlobalHandler::localEntries() const {
	return size_;
}


unsigned int OneDimGlobalHandler::sizeOfDim( unsigned int dim ) const
{
	if ( dim == 0 )
		return size_;
	return 0;
}

char* OneDimGlobalHandler::data( DataId index ) const
{
	return data_ + index.data() * dinfo()->size();
}

bool OneDimGlobalHandler::innerNodeBalance( unsigned int size,
	unsigned int myNode, unsigned int numNodes )
{
	if ( size == size_ )
		return 0;
	else
		size_ = size;
	return 1;
}

bool OneDimGlobalHandler::resize( vector< unsigned int > dims )
{
	if ( dims.size() != 1 ) {
		cout << "OneDimGlobalHandler::Resize: Warning: Attempt to resize wrong # of dims " << dims.size() << "\n";
		return 0;
	}
	if ( !data_ || size_ == 0 ) {
		size_ = dims[0];
		data_ = dinfo()->allocData( size_ );
		return 1;
	}

	unsigned int oldsize = size_;
	if ( nodeBalance( dims[0] ) ) {
		char* newdata = dinfo()->allocData( size_ );
		if ( size_ > oldsize )
			memcpy( newdata, data_, oldsize * dinfo()->size() );
		else
			memcpy( newdata, data_, size_ * dinfo()->size() );
		dinfo()->destroyData( data_ );
		data_ = newdata;
	}
	return ( data_ != 0 );
}

vector< unsigned int > OneDimGlobalHandler::dims() const
{
	vector< unsigned int > ret( 1, size_ );
	return ret;
}


/**
 * Returns true if the node decomposition has the data on the
 * current node
 */
bool OneDimGlobalHandler::isDataHere( DataId index ) const {
	return 1;
}

bool OneDimGlobalHandler::isAllocated() const {
	return data_ != 0;
}

bool OneDimGlobalHandler::isGlobal() const {
	return 1;
}

bool OneDimGlobalHandler::setDataBlock( 
	const char* data, unsigned int numData,
	const vector< unsigned int >& startIndex ) const
{
	if ( startIndex.size() > 1 ) return 0;
	unsigned int s = 0;
	if ( startIndex.size() == 1 )
		s = startIndex[0];
	return setDataBlock( data, numData, s );
}

bool OneDimGlobalHandler::setDataBlock( 
	const char* data, unsigned int numData,
	DataId startIndex ) const
{
	if ( !isAllocated() ) return 0;
	if ( startIndex.data() + numData > size_ ) return 0;
	memcpy( data_ + startIndex.data() * dinfo()->size(), 
		data, numData* dinfo()->size() );
	return 1;
}


void OneDimGlobalHandler::nextIndex( DataId& index,
	unsigned int& linearIndex ) const
{
	index.incrementDataIndex();
	++linearIndex;
}
