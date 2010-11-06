/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "header.h"

const unsigned int Qvec::threadQreserve = 128;

// CPU and OS specific, just pick a biggish value to handle most cases.
// Also it has to be big enough to put in a Qinfo for doing the 
// stitching between blocks.
const unsigned int Qvec::threadOverlapProtection = sizeof( Qinfo );

Qvec::Qvec( unsigned int numThreads )
	: numThreads_( numThreads )
{
	data_.resize( numThreads * threadQreserve, 0 );
	threadBlockStart_.resize( numThreads + 1 );
	threadBlockEnd_.resize( numThreads );

	for ( unsigned int i = 0; i < numThreads_; ++i )
		threadBlockStart_[i] = threadBlockEnd_[i] = i * threadQreserve;
	threadBlockStart_[ numThreads] = numThreads * threadQreserve;
}

void Qvec::push_back( unsigned int thread, const Qinfo* q, const char* arg )
{
	unsigned int tbe = threadBlockEnd_[ thread ];
	unsigned int tbs = threadBlockStart_[ thread + 1 ];
	unsigned int dataSize = sizeof( Qinfo ) + q->size();
	if ( tbe + dataSize + threadOverlapProtection > tbs )
	{
		// Do some clever reallocation here. The goal is to have the
		// blocks reasonably full.
		unsigned int extraSize = dataSize + threadQreserve;
		// insert( pos, n, val );
		data_.insert( data_.begin() + tbe, extraSize, 0 );
		for ( unsigned int i = thread + 1; i < numThreads_; ++i ) {
			threadBlockStart_[i] += extraSize;
			threadBlockEnd_[i] += extraSize;
		}
		threadBlockStart_[ numThreads_ ] += extraSize;
		assert( threadBlockStart_[ numThreads_ ] == data_.size() );
	}
	// Need to figure out pos
	char* pos = &data_[ tbe ];
	memcpy( pos, q, sizeof( Qinfo ) );
	memcpy( pos + sizeof( Qinfo ), arg, q->size() );
	threadBlockEnd_[ thread ] += dataSize;
}

void Qvec::clear()
{
	for ( unsigned int i = 0; i < numThreads_; ++i )
		threadBlockEnd_[i] = threadBlockStart_[i];
}

void Qvec::stitch()
{
	for ( unsigned int i = 0; i < numThreads_; ++i ) {
		// Qinfo( srcIndex, size, useSendTo )
		unsigned int remainingSize = 
			threadBlockStart_[ i + 1 ] - threadBlockEnd_[i];
		assert( remainingSize >= sizeof( Qinfo ) );
		Qinfo q( 0, remainingSize, 0 );
		memcpy( &data_[ threadBlockEnd_[i] ], &q, remainingSize );
	}
}

const char* Qvec::data() const
{
	assert( data_.size() > threadQreserve );
	return &data_[0];
}

char* Qvec::writableData()
{
	assert( data_.size() > threadQreserve );
	return &data_[0];
}

unsigned int Qvec::dataQsize() const
{
	assert( numThreads_ > 0 );
	return threadBlockEnd_[ numThreads_ - 1 ];
}

unsigned int Qvec::allocatedSize() const
{
	return data_.size();
}

// static function
void Qvec::testQvec()
{
	static const unsigned int dat1size = 1234;
	static const unsigned int dat2size = 12;
	Qvec q( 4 );
	assert( q.threadBlockStart_.size() == 5 );
	assert( q.threadBlockEnd_.size() == 4 );
	assert( q.dataQsize() == 3 * threadQreserve );
	assert( q.allocatedSize() == 4 * threadQreserve );
	assert( q.data() == q.writableData() );
	assert( q.data() == &( q.data_[0] ) );
	
	char *buf = new char[5000];
	Qinfo qi( 0, dat1size, buf );
	unsigned int datasize = sizeof( Qinfo ) + dat1size;

	assert( q.threadBlockStart_[0] == 0 );
	assert( q.threadBlockStart_[1] == threadQreserve );
	assert( q.threadBlockStart_[2] == threadQreserve * 2 );
	assert( q.threadBlockStart_[3] == threadQreserve * 3 );
	assert( q.threadBlockStart_[4] == threadQreserve * 4 );
	assert( q.threadBlockEnd_[0] == 0 );
	assert( q.threadBlockEnd_[1] == threadQreserve );
	assert( q.threadBlockEnd_[2] == threadQreserve * 2 );
	assert( q.threadBlockEnd_[3] == threadQreserve * 3 );
	//// Put a big chunk of data on thread 0.
	q.push_back( 0, &qi, buf );
	////
	assert( q.dataQsize() == 4 * threadQreserve + datasize );
	assert( q.allocatedSize() == 5 * threadQreserve + datasize );
	assert( q.threadBlockStart_.size() == 5 );
	assert( q.threadBlockStart_[0] == 0 );
	assert( q.threadBlockStart_[1] == threadQreserve * 2 + datasize );
	assert( q.threadBlockStart_[2] == threadQreserve * 3 + datasize );
	assert( q.threadBlockStart_[3] == threadQreserve * 4 + datasize );
	assert( q.threadBlockStart_[4] == threadQreserve * 5 + datasize );
	assert( q.threadBlockEnd_[0] == datasize );
	assert( q.threadBlockEnd_[1] == threadQreserve * 2 + datasize  );
	assert( q.threadBlockEnd_[2] == threadQreserve * 3 + datasize );
	assert( q.threadBlockEnd_[3] == threadQreserve * 4 + datasize );

	//// Put some more data (small) on thread 0.
	Qinfo qiSmall( 0, dat2size, buf );
	unsigned int datasizeSmall = sizeof( Qinfo ) + dat2size;
	q.push_back( 0, &qiSmall, buf );
	////
	assert( q.dataQsize() == 4 * threadQreserve + datasize );
	assert( q.allocatedSize() == 5 * threadQreserve + datasize );
	assert( q.threadBlockStart_.size() == 5 );
	assert( q.threadBlockStart_[0] == 0 );
	assert( q.threadBlockStart_[1] == threadQreserve * 2 + datasize );
	assert( q.threadBlockStart_[2] == threadQreserve * 3 + datasize );
	assert( q.threadBlockStart_[3] == threadQreserve * 4 + datasize );
	assert( q.threadBlockStart_[4] == threadQreserve * 5 + datasize );
	assert( q.threadBlockEnd_[0] == datasize + datasizeSmall );
	assert( q.threadBlockEnd_[1] == threadQreserve * 2 + datasize  );
	assert( q.threadBlockEnd_[2] == threadQreserve * 3 + datasize );
	assert( q.threadBlockEnd_[3] == threadQreserve * 4 + datasize );

	//// Put some data on thread 1
	q.push_back( 1, &qiSmall, buf );
	////
	assert( q.dataQsize() == 4 * threadQreserve + datasize );
	assert( q.allocatedSize() == 5 * threadQreserve + datasize );
	assert( q.threadBlockStart_.size() == 5 );
	assert( q.threadBlockStart_[0] == 0 );
	assert( q.threadBlockStart_[1] == threadQreserve * 2 + datasize );
	assert( q.threadBlockStart_[2] == threadQreserve * 3 + datasize );
	assert( q.threadBlockStart_[3] == threadQreserve * 4 + datasize );
	assert( q.threadBlockStart_[4] == threadQreserve * 5 + datasize );
	assert( q.threadBlockEnd_[0] == datasize + datasizeSmall );
	assert( q.threadBlockEnd_[1] == 
		threadQreserve * 2 + datasize + datasizeSmall  );
	assert( q.threadBlockEnd_[2] == threadQreserve * 3 + datasize );
	assert( q.threadBlockEnd_[3] == threadQreserve * 4 + datasize );

	const char* data = q.data();
	const Qinfo* temp = reinterpret_cast< const Qinfo* >( data );
	assert( temp->size() == dat1size );
	temp = reinterpret_cast< const Qinfo* >( data + datasize );
	assert( temp->size() == dat2size );

	temp = reinterpret_cast< const Qinfo* >( data + q.threadBlockStart_[1] );
	assert( temp->size() == dat2size );

	/////// Now check function of the 'stitch' command.

	delete[] buf;

	cout << "+" << flush;
}
