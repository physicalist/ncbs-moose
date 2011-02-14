/**********************************************************************
 ** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "AssignmentMsg.h"
#include "AssignVecMsg.h"
#include "SingleMsg.h"
#include "DiagonalMsg.h"
#include "OneToOneMsg.h"
#include "OneToAllMsg.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "ReduceMsg.h"

#include "ReduceBase.h"
#include "ReduceMax.h"
#include "../shell/Shell.h"

///////////////////////////////////////////////////////////////////////////

// Static field declaration.
vector< Msg* > Msg::msg_;
vector< MsgId > Msg::garbageMsg_;
const MsgId Msg::badMsg = 0;
const MsgId Msg::setMsg = 1;
Id Msg::msgManagerId_;

Msg::Msg( MsgId mid, Element* e1, Element* e2, Id managerId )
	: mid_( mid), e1_( e1 ), e2_( e2 )
{
	// assert( mid_ < msg_.size() );
	if ( mid_ >= msg_.size() ) {
		msg_.resize( mid + 1, 0 );
	}
	msg_[mid_] = this;
	e1->addMsg( mid_ );
	e2->addMsg( mid_ );
}

Msg::~Msg()
{
	msg_[ mid_ ] = 0;
	e1_->dropMsg( mid_ );
	e2_->dropMsg( mid_ );

	if ( mid_ > 1 )
		garbageMsg_.push_back( mid_ );
}

/**
 * Returns a MsgId for assigning to a new Msg.
 */
MsgId Msg::nextMsgId()
{
	MsgId ret;
	if ( garbageMsg_.size() > 0 ) {
		ret = garbageMsg_.back();
		garbageMsg_.pop_back();
	} else {
		ret = msg_.size();
		msg_.push_back( 0 );
	}
	return ret;
}

void Msg::deleteMsg( MsgId mid )
{
	assert( mid < msg_.size() );
	Msg* m = msg_[ mid ];
	if ( m != 0 )
		delete m;
	msg_[ mid ] = 0;
}

/**
 * Initialize the Null location in the Msg vector.
 */
void Msg::initNull()
{
	assert( msg_.size() == 0 );
	nextMsgId(); // Set aside entry 0 for badMsg;
	nextMsgId(); // Set aside entry 1 for setMsg;
}

void Msg::process( const ProcInfo* p, FuncId fid ) const 
{
	e2_->process( p, fid );
}

const Msg* Msg::getMsg( MsgId m )
{
	assert( m < msg_.size() );
	return msg_[ m ];
}

Msg* Msg::safeGetMsg( MsgId m )
{
	if ( m == badMsg )
		return 0;
	if ( m < msg_.size() )
		return msg_[ m ];
	return 0;
}

Eref Msg::manager() const
{
	return Eref( ( this->managerId() )(), DataId( mid_ ) );
}

/*
void Msg::setDataId( unsigned int di ) const
{
	assert( lookupDataId_.size() > mid_ );
	lookupDataId_[ mid_ ] = di;
}
*/

void Msg::addToQ( const Element* src, Qinfo& q,
	const ProcInfo* p, MsgFuncBinding i, const char* arg ) const
{
	if ( e1_ == src ) {
		q.addToQforward( p, i, arg );
	} else {
		q.addToQbackward( p, i, arg ); 
	}
}

// Static func
unsigned int Msg::numMsgs()
{
	return msg_.size();
}

/**
 * Return the first element id
 */
Id Msg::getE1() const
{
	return e1_->id();
}

/**
 * Return the second element id
 */
Id Msg::getE2() const
{
	return e2_->id();
}

///////////////////////////////////////////////////////////////////////////
// Here we set up the Element related stuff for Msgs.
///////////////////////////////////////////////////////////////////////////

const Cinfo* Msg::initCinfo()
{
	///////////////////////////////////////////////////////////////////
	// Field definitions.
	///////////////////////////////////////////////////////////////////
	static ReadOnlyValueFinfo< Msg, Id > e1(
		"e1",
		"Id of source Element.",
		&Msg::getE1
	);
	static ReadOnlyValueFinfo< Msg, Id > e2(
		"e2",
		"Id of source Element.",
		&Msg::getE2
	);

	static Finfo* msgFinfos[] = {
		&e1,		// readonly value
		&e2,		// readonly value
	};

	static Cinfo msgCinfo (
		"Msg",	// name
		Neutral::initCinfo(),				// base class
		msgFinfos,
		sizeof( msgFinfos ) / sizeof( Finfo* ),	// num Fields
		0
		// new Dinfo< Msg >()
	);

	return &msgCinfo;
}

static const Cinfo* msgCinfo = Msg::initCinfo();

// Static func
void Msg::initMsgManagers()
{
	vector< unsigned int > dims( 1, 0 );

	// This is to be the parent of all the msg managers.
	msgManagerId_ = Id::nextId();
	new Element( msgManagerId_, Neutral::initCinfo(), "Msgs", dims, 1 );

	SingleMsg::managerId_ = Id::nextId();
	new Element( SingleMsg::managerId_, SingleMsg::initCinfo(), "singleMsg", dims, 1 );

	Shell::adopt( Id(), msgManagerId_ );
	Shell::adopt( msgManagerId_, SingleMsg::managerId_ );

	OneToOneMsg::managerId_ = Id::nextId();
	new Element( OneToOneMsg::managerId_, OneToOneMsg::initCinfo(), "oneToOneMsg", dims, 1 );
	Shell::adopt( msgManagerId_, OneToOneMsg::managerId_ );

	OneToAllMsg::managerId_ = Id::nextId();
	new Element( OneToAllMsg::managerId_, OneToAllMsg::initCinfo(), "oneToAllMsg", dims, 1 );
	Shell::adopt( msgManagerId_, OneToAllMsg::managerId_ );
	DiagonalMsg::managerId_ = Id::nextId();
	new Element( DiagonalMsg::managerId_, DiagonalMsg::initCinfo(), "diagonalMsg", dims, 1 );
	Shell::adopt( msgManagerId_, DiagonalMsg::managerId_ );
	SparseMsg::managerId_ = Id::nextId();
	new Element( SparseMsg::managerId_, SparseMsg::initCinfo(), "sparseMsg", dims, 1 );
	Shell::adopt( msgManagerId_, SparseMsg::managerId_ );
	AssignmentMsg::managerId_ = Id::nextId();
	new Element( AssignmentMsg::managerId_, AssignmentMsg::initCinfo(), "assignmentMsg", dims, 1 );
	Shell::adopt( msgManagerId_, AssignmentMsg::managerId_ );
	AssignVecMsg::managerId_ = Id::nextId();
	new Element( AssignVecMsg::managerId_, AssignVecMsg::initCinfo(), "assignVecMsg", dims, 1 );
	Shell::adopt( msgManagerId_, AssignVecMsg::managerId_ );
	ReduceMsg::managerId_ = Id::nextId();
	new Element( ReduceMsg::managerId_, ReduceMsg::initCinfo(), "ReduceMsg", dims, 1 );
	Shell::adopt( msgManagerId_, ReduceMsg::managerId_ );
}
