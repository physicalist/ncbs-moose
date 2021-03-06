/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ReduceFinfo.h"
#ifdef USE_MPI
#include <mpi.h>
#include "ReduceMsg.h"
#include "../shell/Shell.h"
#endif

ReduceBase::ReduceBase() // To keep vectors happy
	: srcId_( Id() ), rfb_( 0 ), inited_( 0 )
{;}

ReduceBase::ReduceBase( ObjId srcId, const ReduceFinfoBase* rfb )
	: srcId_( srcId ), rfb_( rfb ), inited_( 0 )
{;}

ReduceBase::~ReduceBase()
{;}

bool ReduceBase::sameEref( const ReduceBase* other ) const
{
	return ( rfb_ == other->rfb_ && srcId_ == other->srcId_ );
}

bool ReduceBase::reduceNodes()
{
#ifdef USE_MPI
	char* recvBuf = new char[ Shell::numNodes() * this->dataSize() ];
	char* sendBuf = new char[ this->dataSize() ];
	/*
	 * Printf debugging
	if ( this->dataSize() == ( 24 )){ // Checked, this is size of ReduceDataType
		const char* temp = this->data();
		const double *x = reinterpret_cast< const double *>( temp );
		const unsigned int* j = reinterpret_cast< const unsigned int*>( temp + 2 * sizeof( double ));
		cout << Shell::myNode() << ": local data entry =( " << 
				*x << ", " << *j << " )\n";
	}
	 */

	memcpy( sendBuf, this->data(), this->dataSize() );
	MPI_Allgather( sendBuf, this->dataSize(), MPI_CHAR, 
		recvBuf, this->dataSize(), MPI_CHAR, 
		MPI_COMM_WORLD );
	for ( unsigned int i = 0; i < Shell::numNodes(); ++i ) {
		if ( i != Shell::myNode() )
			this->tertiaryReduce( recvBuf + i * this->dataSize() );
	}
	/*
	* Printf debugging
	if ( this->dataSize() == ( 24 )){ // Checked, this is size of ReduceDataType
		const char* temp = recvBuf;
		for ( unsigned int i = 0; i < Shell::numNodes(); ++i ) {
			const double *x = reinterpret_cast< const double *>( temp );
			const unsigned int* j = reinterpret_cast< const unsigned int*>( temp + 2 * sizeof( double ));
			cout << Shell::myNode() << ": recvBuf entry " << i << "=( " << 
				*x << ", " << *j << " )\n";
			temp += this->dataSize();
		}
	}
	*/
	delete[] recvBuf;
	delete[] sendBuf;
#endif
	
	return srcId_.isDataHere(); // Do we need to assign the result here?
}

void ReduceBase::assignResult() const
{
	rfb_->digestReduce( srcId_.eref(), this );
}

bool ReduceBase::getInited() const
{
	return inited_;
}

void ReduceBase::setInited()
{
	inited_ = true;
}

/////////////////////////////////////////////////////////////////////////

// The function is set up by a suitable SetGet templated wrapper.
ReduceStats::ReduceStats( ObjId srcId, const ReduceFinfoBase* rfb, 
	const GetOpFuncBase< double >* gof )
	: 
		ReduceBase( srcId, rfb ),
		gof_( gof )
{
	data_.sum_ = 0.0;
	data_.sumsq_ = 0.0;
	data_.count_ = 0 ;
}

ReduceStats::~ReduceStats()
{;}

const char* ReduceStats::data() const
{
	return reinterpret_cast< const char* >( &data_ );
}

unsigned int ReduceStats::dataSize() const
{
	return sizeof( ReduceDataType );
}

void ReduceStats::primaryReduce( ObjId tgtId )
{
	double x = gof_->reduceOp( tgtId.eref() );
	data_.sum_ += x;
	data_.sumsq_ += x * x;
	data_.count_++;
}

// Must not use other::func_
void ReduceStats::secondaryReduce( const ReduceBase* other )
{
	const ReduceStats* r = dynamic_cast< const ReduceStats* >( other );
	assert( r );
	data_.sum_ += r->data_.sum_;
	data_.sumsq_ += r->data_.sumsq_;
	data_.count_ += r->data_.count_;
}

// Must not use other::func_
void ReduceStats::tertiaryReduce( const char* other )
{
	const ReduceDataType* d = reinterpret_cast< const ReduceDataType* >( other );
	assert( d );

	// cout << Shell::myNode() << ": ReduceStats::tertiaryReduce: stats = " << data_.sum_ << ", " << data_.sumsq_ << ", " << data_.count_ << endl;

	data_.sum_ += d->sum_;
	data_.sumsq_ += d->sumsq_;
	data_.count_ += d->count_;

	// cout << Shell::myNode() << ": ReduceStats::tertiaryReduce: stats = " << data_.sum_ << ", " << data_.sumsq_ << ", " << data_.count_ << endl;
}

double ReduceStats::sum() const
{
	return data_.sum_;
}

double ReduceStats::sumsq() const
{
	return data_.sumsq_;
}

unsigned int ReduceStats::count() const
{
	return data_.count_;
}

/////////////////////////////////////////////////////////////////////////

// The function is set up by a suitable SetGet templated wrapper.
ReduceFieldDimension::ReduceFieldDimension( ObjId srcId, const ReduceFinfoBase* rfb, 
	const GetOpFuncBase< unsigned int >* gof )
	: 
		ReduceBase( srcId, rfb ),
		maxIndex_( 0 ),
		gof_( gof )
{;}

ReduceFieldDimension::~ReduceFieldDimension()
{;}

const char* ReduceFieldDimension::data() const
{
	return reinterpret_cast< const char* >( &maxIndex_ );
}

unsigned int ReduceFieldDimension::dataSize() const
{
	return sizeof( unsigned int );
}

void ReduceFieldDimension::primaryReduce( ObjId tgtId )
{
	unsigned int i = gof_->reduceOp( tgtId.eref() );
	tgtId_ = tgtId;
	if ( i > maxIndex_ )
		maxIndex_ = i;
}

// Must not use other::func_
void ReduceFieldDimension::secondaryReduce( const ReduceBase* other )
{
	const ReduceFieldDimension* r = 
		dynamic_cast< const ReduceFieldDimension* >( other );
	assert( r );
	if ( r->maxIndex_ > maxIndex_ )
		maxIndex_ = r->maxIndex_;
	if ( ( !getInited() ) && r->getInited() ) {
		tgtId_ = r->tgtId_;
		setInited();
	}
}

// Must not use other::func_
void ReduceFieldDimension::tertiaryReduce( const char* other )
{
	const unsigned int m = *reinterpret_cast< const unsigned int* >( other);
	if ( m > maxIndex_ )
		maxIndex_ = m;
}

unsigned int ReduceFieldDimension::maxIndex() const
{
	return maxIndex_;
}

bool ReduceFieldDimension::reduceNodes()
{
	assert ( getInited() );
	bool ret = ReduceBase::reduceNodes();
	// cout << Shell::myNode() << ": ReduceFieldDimension::reduceNodes: maxindex= " << maxIndex_ << ", tgt=" << tgtId_ << endl;
	
	tgtId_.element()->dataHandler()->setFieldDimension( maxIndex_ );
	return ret;
}
