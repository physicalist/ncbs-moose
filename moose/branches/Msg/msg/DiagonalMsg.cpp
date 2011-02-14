/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "DiagonalMsg.h"

Id DiagonalMsg::managerId_;

DiagonalMsg::DiagonalMsg( MsgId mid, Element* e1, Element* e2 )
	: Msg( mid, e1, e2, DiagonalMsg::managerId_ ), 
	stride_( 1 )
{
	;
}

DiagonalMsg::~DiagonalMsg()
{
	;
}

/**
 * This is sort of thread-safe, as there is only ever one target for
 * any given input. Furthermore, this target will always be unique:
 * no other input will hit the same target. So we can partition the
 * message exec operation among sources as we like.
 */
void DiagonalMsg::exec( const char* arg, const ProcInfo *p ) const
{
	const Qinfo *q = ( reinterpret_cast < const Qinfo * >( arg ) );

	int src = q->srcIndex().data();
	if ( q->isForward() ) {
		int dest = src + stride_;
		if ( dest >= 0 && e2_->dataHandler()->isDataHere( dest ) ) {
			if ( p->execThread( e2_->id(), dest ) ) {
				const OpFunc* f = e2_->cinfo()->getOpFunc( q->fid() );
				f->op( Eref( e2_, dest ), arg );
			}
		}
	} else {
		// Here we are stuck a bit. I will assume srcIndex is now for e2
		int dest = src - stride_;
		if ( dest >= 0 && e1_->dataHandler()->isDataHere( dest ) ) {
			if ( p->execThread( e1_->id(), dest ) ) {
				const OpFunc* f = e1_->cinfo()->getOpFunc( q->fid() );
				f->op( Eref( e1_, dest ), arg );
			}
		}
	}
}

Id DiagonalMsg::managerId() const
{
	return DiagonalMsg::managerId_;
}

void DiagonalMsg::setStride( int stride )
{
	stride_ = stride;
}

int DiagonalMsg::getStride() const
{
	return stride_;
}

FullId DiagonalMsg::findOtherEnd( FullId f ) const
{
	if ( f.id() == e1() ) {
		int i2 = f.dataId.data() + stride_;
		if ( i2 >= 0 ) {
			unsigned int ui2 = i2;
			if ( ui2 < e2()->dataHandler()->totalEntries() )
				return FullId( e2()->id(), DataId( ui2, f.dataId.field() ) );
		}
		return FullId( e2()->id(), DataId::bad() );
	} else if ( f.id() == e2() ) {
		int i1 = f.dataId.data() - stride_;
		if ( i1 >= 0 ) {
			unsigned int ui1 = i1;
			if ( ui1 < e1()->dataHandler()->totalEntries() )
				return FullId( e1()->id(), DataId( ui1, f.dataId.field() ));
		}
		return FullId( e1()->id(), DataId::bad() );
	}
	return FullId::bad();
}

Msg* DiagonalMsg::copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const
{
	const Element* orig = origSrc();
	if ( n <= 1 ) {
		DiagonalMsg* ret;
		if ( orig == e1() )
			ret = new DiagonalMsg( Msg::nextMsgId(), newSrc(), newTgt() );
		else if ( orig == e2() )
			ret = new DiagonalMsg( Msg::nextMsgId(), newTgt(), newSrc() );
		else
			assert( 0 );
		ret->setStride( stride_ );
		ret->e1()->addMsgAndFunc( ret->mid(), fid, b );
		return ret;
	} else {
		// Here we need a SliceMsg which goes from one 2-d array to another.
		cout << "Error: DiagonalMsg::copy: DiagonalSliceMsg not yet implemented\n";
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////
// Here we set up the MsgManager portion of the class.
///////////////////////////////////////////////////////////////////////

const Cinfo* DiagonalMsg::initCinfo()
{
	///////////////////////////////////////////////////////////////////
	// Field definitions.
	///////////////////////////////////////////////////////////////////
	static ValueFinfo< DiagonalMsg, int > stride(
		"stride",
		"The stride is the increment to the src DataId that gives the"
		"dest DataId. "
		"It can be positive or negative, but bounds checking"
		"takes place and it does not wrap around.",
		&DiagonalMsg::setStride,
		&DiagonalMsg::getStride
	);

	static Finfo* msgFinfos[] = {
		&stride,		// value
	};

	static Cinfo msgCinfo (
		"DiagonalMsg",	// name
		Msg::initCinfo(),				// base class
		msgFinfos,
		sizeof( msgFinfos ) / sizeof( Finfo* ),	// num Fields
		new Dinfo< short >()
	);

	return &msgCinfo;
}

static const Cinfo* assignmentMsgCinfo = DiagonalMsg::initCinfo();
