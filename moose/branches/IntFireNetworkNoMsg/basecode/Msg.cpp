/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "Message.h"

///////////////////////////////////////////////////////////////////////////
Msg::Msg( Element* e1, Element* e2 )
	: e1_( e1 ), e2_( e2 )
{
	m1_ = e1->addMsg( this );
	m2_ = e2->addMsg( this );
}

Msg::~Msg()
{
	e1_->dropMsg( this, m1_ );
	e2_->dropMsg( this, m2_ );
}

void Msg::clearQ() const 
{
	e1_->clearQ();
}

void Msg::process( const ProcInfo* p ) const 
{
	e1_->process( p );
}

void Msg::addToQ( const Element* caller, Qinfo& q, const char* arg ) const
{
	if ( caller == e1_ ) {
		q.setMsgId( m2_ );
		e2_->addToQ( q, arg );
	} else {
		assert( caller == e2_ );
		q.setMsgId( m1_ );
		e1_->addToQ( q, arg );
	}
}

///////////////////////////////////////////////////////////////////////////

SingleMsg::SingleMsg( Eref e1, Eref e2 )
	: Msg( e1.element(), e2.element() ),
	i1_( e1.index() ), 
	i2_( e2.index() )
{
	;
}

void SingleMsg::exec( Element* target, const char* arg ) const
{
	const Qinfo *q = ( reinterpret_cast < const Qinfo * >( arg ) );
	// arg += sizeof( Qinfo );
	const OpFunc* f = target->cinfo()->getOpFunc( q->fid() );
	if ( target == e1_ ) {
		f->op( Eref( target, i1_ ), arg );
	} else {
		assert( target == e2_ );
		f->op( Eref( target, i2_ ), arg );
	}
}

/*
const char* SingleMsg::exec( Element* target, const OpFunc* f, 
		unsigned int srcIndex, const char* arg ) const
{
	if ( target == e1_ ) {
		f->op( Eref( target, i1_ ), arg );
	} else {
		assert( target == e2_ );
		f->op( Eref( target, i2_ ), arg );
	}
	return 0;
}
*/

bool SingleMsg::add( Eref e1, const string& srcField, 
			Eref e2, const string& destField )
{
	FuncId funcId;
	const SrcFinfo* srcFinfo = validateMsg( e1.element(), srcField,
		e2.element(), destField, funcId );

	if ( srcFinfo ) {
		Msg* m = new SingleMsg( e1, e2 );
		e1.element()->addMsgToConn( m, srcFinfo->getConnId() );
		e1.element()->addTargetFunc( funcId, srcFinfo->getFuncIndex() );
		return 1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////

OneToOneMsg::OneToOneMsg( Element* e1, Element* e2 )
	: Msg( e1, e2 )
{
	;
}

/**
 * Possible issue here if srcIndex is not a DataId
 */
void OneToOneMsg::exec( Element* target, const char* arg ) const
{
	const Qinfo *q = ( reinterpret_cast < const Qinfo * >( arg ) );
	// arg += sizeof( Qinfo );
	const OpFunc* f = target->cinfo()->getOpFunc( q->fid() );
	assert( target == e1_ || target == e2_ );
	f->op( Eref( target, q->srcIndex() ), arg );
}


/*
const char* OneToOneMsg::exec( Element* target, const OpFunc* f, 
		unsigned int srcIndex, const char* arg ) const
{
	if ( target == e1_ ) {
		f->op( Eref( target, srcIndex ), arg );
	} else {
		assert( target == e2_ );
		f->op( Eref( target, srcIndex ), arg );
	}
	return 0;
}
*/
