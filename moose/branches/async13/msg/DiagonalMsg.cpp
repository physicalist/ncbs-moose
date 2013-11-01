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

// Static field declaration
Id DiagonalMsg::managerId_;

DiagonalMsg::DiagonalMsg( MsgId mid, Element* e1, Element* e2 )
	: Msg( mid, e1, e2, DiagonalMsg::managerId_ ), 
	stride_( 1 )
{
	;
}

DiagonalMsg::~DiagonalMsg()
{
	destroyDerivedMsg( managerId_, mid_ );
}

Eref DiagonalMsg::firstTgt( const Eref& src ) const 
{
	if ( src.element() == e1_ ) {
		unsigned int nextData = src.index() + stride_;
		return Eref( e2_, nextData );
	}
	else if ( src.element() == e2_ ) {
		unsigned int nextData = src.index() - stride_;
		return Eref( e1_, nextData );
	}
	return Eref( 0, 0 );
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

ObjId DiagonalMsg::findOtherEnd( ObjId f ) const
{
	if ( f.id() == e1() ) {
		int i2 = f.dataId + stride_;
		if ( i2 >= 0 ) {
			unsigned int ui2 = i2;
			if ( ui2 < e2()->numData() )
				return ObjId( e2()->id(), DataId( ui2 ) );
		}
		return ObjId( Id() );
	} else if ( f.id() == e2() ) {
		int i1 = f.dataId - stride_;
		if ( i1 >= 0 ) {
			unsigned int ui1 = i1;
			if ( ui1 < e1()->numData() )
				return ObjId( e1()->id(), DataId( ui1 ));
		}
		return ObjId( Id() );
	}
	return ObjId::bad();
}

Msg* DiagonalMsg::copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const
{
	const Element* orig = origSrc();
	if ( n <= 1 ) {
		DiagonalMsg* ret = 0;
		if ( orig == e1() ) {
			ret = new DiagonalMsg( Msg::nextMsgId(), newSrc(), newTgt() );
			ret->e1()->addMsgAndFunc( ret->mid(), fid, b );
		} else if ( orig == e2() ) {
			ret = new DiagonalMsg( Msg::nextMsgId(), newTgt(), newSrc() );
			ret->e2()->addMsgAndFunc( ret->mid(), fid, b );
		} else {
			assert( 0 );
		}
		ret->setStride( stride_ );
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
