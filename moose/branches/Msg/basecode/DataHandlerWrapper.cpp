/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "DataHandlerWrapper.h"

DataHandlerWrapper::DataHandlerWrapper( const DataHandler* parentHandler )
	: DataHandler( parentHandler->dinfo() ), parent_( parentHandler )
{
}

DataHandlerWrapper::~DataHandlerWrapper()
{;} // This is the key function. It deletes itself but does not touch parent


DataHandler* DataHandlerWrapper::globalize() const
{
	return parent_->globalize();
}

DataHandler* DataHandlerWrapper::unGlobalize() const
{
	return parent_->unGlobalize();
}

bool DataHandlerWrapper::nodeBalance( unsigned int size )
{
	return 0;
}

DataHandler* DataHandlerWrapper::copy() const
{
	return parent_->copy();
}

DataHandler* DataHandlerWrapper::copyExpand( unsigned int copySize ) const
{
	return parent_->copyExpand( copySize );
}

DataHandler* DataHandlerWrapper::copyToNewDim( unsigned int newDimSize ) const
{
	return parent_->copyToNewDim( newDimSize );
}

char* DataHandlerWrapper::data( DataId index ) const {
	return parent_->data( index );
}

void DataHandlerWrapper::process( const ProcInfo* p, Element* e, FuncId fid ) const
{
	parent_->process( p, e, fid );
}

/**
 * Returns the number of data entries.
 */
unsigned int DataHandlerWrapper::totalEntries() const {
	return parent_->totalEntries();
}

/**
 * Returns the number of dimensions of the data.
 */
unsigned int DataHandlerWrapper::numDimensions() const {
	return parent_->numDimensions();
}

unsigned int DataHandlerWrapper::sizeOfDim( unsigned int dim ) const {
	return parent_->sizeOfDim( dim );
}

// Cannot do, as it modifies parent.
bool DataHandlerWrapper::resize( vector< unsigned int > dims ) {
	return 0;
	// return parent_->resize( dims );
}

vector< unsigned int > DataHandlerWrapper::dims() const {
	return parent_->dims();
}

bool DataHandlerWrapper::isDataHere( DataId index ) const {
	return parent_->isDataHere( index );
}

bool DataHandlerWrapper::isAllocated() const {
	return parent_->isAllocated();
}

bool DataHandlerWrapper::isGlobal() const
{
	return parent_->isGlobal();
}

DataHandler::iterator DataHandlerWrapper::begin() const
{
	return parent_->begin();
}

DataHandler::iterator DataHandlerWrapper::end() const
{
	return parent_->end();
}

bool DataHandlerWrapper::setDataBlock( const char* data, 
	unsigned int numEntries, DataId startIndex ) const
{
	return parent_->setDataBlock( data, numEntries, startIndex );
}

bool DataHandlerWrapper::setDataBlock( 
	const char* data, unsigned int numEntries, 
	const vector< unsigned int >& startIndex ) const
{
	return parent_->setDataBlock( data, numEntries, startIndex );
}

unsigned int DataHandlerWrapper::nextIndex( unsigned int index ) const
{
	return parent_->nextIndex( index );
}
