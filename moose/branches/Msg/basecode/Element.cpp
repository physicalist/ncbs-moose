/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"

Element::Element( const Cinfo* c )
	: cinfo_( c )
{ ; }

/*
Element::Element( vector< Data* >& d, 
	unsigned int numSendSlots, unsigned int numRecvSlots )
	: d_( d ), 
	finfo_( d_[0]->initClassInfo() ), 
	numSendSlots_( numSendSlots ),
	numRecvSlots_( numRecvSlots )
{
	q_.resize( 16, 0 ); // Put in place space for at least one entry.
}
*/

Element::~Element()
{
	delete[] sendBuf_;
	for ( vector< Data* >::iterator i = d_.begin(); i != d_.end(); ++i )
		delete *i;
	for ( vector< Msg* >::iterator i = m_.begin(); i != m_.end(); ++i )
		delete *i;
}

void Element::process( const ProcInfo* p )
{
	for ( unsigned int i = 0; i < d_.size(); ++i )
		d_[i]->process( p, Eref( this, i ) );
}


double Element::sumBuf( SyncId slot, unsigned int i ) const
{
	vector< unsigned int >::const_iterator offset = 
		procBufRange_.begin() + slot + i * numRecvSlots_;
	vector< double* >::const_iterator begin = 
		procBuf_.begin() + *offset++;
	vector< double* >::const_iterator end = 
		procBuf_.begin() + *offset;
	double ret = 0.0;
	for ( vector< double* >::const_iterator i = begin; 
		i != end; ++i )
		ret += **i;
	return ret;
}

double Element::prdBuf( SyncId slot, unsigned int i, double v )
	const
{
	vector< unsigned int >::const_iterator offset = 
		procBufRange_.begin() + slot + i * numRecvSlots_;
	vector< double* >::const_iterator begin = 
		procBuf_.begin() + *offset++;
	vector< double* >::const_iterator end = 
		procBuf_.begin() + *offset;
	for ( vector< double* >::const_iterator i = begin;
		i != end; ++i )
		v *= **i;
	return v;
}

double Element::oneBuf( SyncId slot, unsigned int i ) const
{
	// unsigned int offset = i * numData_ + slot;
	unsigned int offset = slot + i * numRecvSlots_;
	assert( offset + 1 < procBufRange_.size() );
	return *procBuf_[ procBufRange_[ offset ] ];
}

double* Element::getBufPtr( SyncId slot, unsigned int i )
{
	// unsigned int offset = i * numData_ + slot;
	unsigned int offset = slot + i * numRecvSlots_;
	assert( offset + 1 < procBufRange_.size() );
	return procBuf_[ procBufRange_[ offset ] ];
}

void Element::ssend1( SyncId slot, unsigned int i, double v )
{
	sendBuf_[ slot + i * numSendSlots_ ] = v;
}

void Element::ssend2( SyncId slot, unsigned int i, double v1, double v2 )
{
	double* sb = sendBuf_ + slot + i * numSendSlots_;
	*sb++ = v1;
	*sb = v2;
}

Data* Element::data( unsigned int index )
{
	assert( index < d_.size() );
	return d_[ index ];
}

/*
const vector< Msg* >& Element::msg( SyncId slot ) const
{
	assert( msg_.size() > slot );
	return msg_[ slot ];
}
*/

const Conn& Element::conn( ConnId c ) const {
	assert( c < c_.size() );
	return c_[c];
}

const Msg* Element::getMsg( MsgId mid ) const {
	assert( mid < m_.size() );
	return m_[ mid ];
}

/**
 * Parses the buffer and executes the func in all specified Data
 * objects on the Element.
 * Returns new position on buffer.
 * The buffer looks like this:
 * uint FuncId, uint MsgId, Args
 *
 * The Msg does the iteration, and as it is a virtual base class
 * it can do all sorts of optimizations depending on how the mapping looks.
 *
 */
const char* Element::execFunc( const char* buf )
{
	assert( buf != 0 );
	FuncId fid = *( reinterpret_cast < const FuncId * >( buf ) );
	buf += sizeof( FuncId );
	MsgId mid = *reinterpret_cast< const MsgId* >( buf );
	buf += sizeof( MsgId );
	OpFunc func = cinfo_->getOpFunc( fid ); // Runtime check for type safety
	const Msg* m = getMsg( mid ); // Runtime check for Msg identity.
	if ( func && m )
		return m->exec( this, func, buf );

	return 0;
}

/**
 * clearQ: goes through async function request queue and carries out
 * operations on it. 
 *
 * Node decomposition of Element: Two stage process. First, the Queue
 * request itself must reach all target nodes. This will have to be 
 * managed by the Connection and set up with the Element when the
 * Connection is created. Second, the Element must provide the Msg
 * with range info. Msg will use efficiently to choose what to call.
 *
 * Thread decomposition of Element:	Incoming data is subdivided into
 * per-thread buffers. It would be nice to clearQ these per-thread too.
 * Need some way to shift stuff around for balancing. Must ensure that
 * only one Msg deals with any given index.
 *
 */
void Element::clearQ( )
{
	const char* buf = &(q_[0]);
	while ( buf && *reinterpret_cast< const FuncId* >(buf) != ENDFUNC ) {
		buf = execFunc( buf );
	}
}

/**
 * This function pushes a function event onto the queue.
 * It should be extended to provide thread safety, which can be done
 * if each thread has its own queue.
 */
void Element::addToQ( FuncId funcId, MsgId msgId, const char* arg, unsigned int size )
{
	if ( q_.size() < 8 + size )
		q_.resize( q_.size() + 16 + size * 2 );
	char* pos = &q_[ q_.size()  - 1 ];
	*reinterpret_cast< FuncId* >( pos ) = funcId;
	*reinterpret_cast< FuncId* >( pos + sizeof( FuncId) ) = msgId;
	memcpy( pos + sizeof( FuncId ) * 2, arg, size );
}
