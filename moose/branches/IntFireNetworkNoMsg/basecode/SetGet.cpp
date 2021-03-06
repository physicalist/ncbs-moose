/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "Shell.h"
#include "SetGet.h"

Eref SetGet::shelle_( 0, 0 );
Element* SetGet::shell_;

void SetGet::setShell()
{
	Id shellid;
	shelle_ = shellid.eref();
	shell_ = shelle_.element();
}

/**
 * completeSet: Confirms that the target function has been executed.
 * Later this has to be much more complete: perhaps await return of a baton
 * to ensure that the operation is complete through varous threading,
 * nesting, and multinode operations.
 * Even single thread, single node this current version is dubious: 
 * suppose the function called by 'set' issues a set command of its own?
 */
void SetGet::completeSet() const
{
	e_.element()->clearQ();
}

bool SetGet::checkSet( const string& destField, FuncId& fid ) const
{
	string field = "set_" + destField;
	fid = e_.element()->cinfo()->getOpFuncId( field );
	const OpFunc* func = e_.element()->cinfo()->getOpFunc( fid );
	if ( !func ) {
		cout << "set::Failed to find " << e_ << "." << field << endl;
		return 0;
	}
	if ( func->checkSet( this ) ) {
		return 1;
	} else {
		cout << "set::Type mismatch" << e_ << "." << field << endl;
		return 0;
	}
}

/**
 * Puts data into target queue for calling functions and setting
 * fields. This is a common core function used by the various
 * type-specialized variants.
 * Called after func checking etc.
 */
void SetGet::iSetInner( FuncId fid, const char* val, unsigned int size )
{
	static ConnId setCid = 0;
	static unsigned int setFuncIndex = 0;
	shell_->clearConn( setCid );
	Msg* m = new SingleMsg( shelle_, e_ );
	/*
	if ( e_.index().toAll() ) {
		Msg* m = new SingleMsg( shelle_, e_ );
	} else {
		Msg* m = new OneToAllMsg( shelle_, e_ );
	}
	*/
	shell_->addMsgToConn( m, setCid );
	shell_->addTargetFunc( fid, setFuncIndex );
	shelle_.asend( setCid, setFuncIndex, val, size );
}

void SetGet::clearQ() const
{
	e_.element()->clearQ();
}

void SetGet::resizeBuf( unsigned int size )
{
	buf_.resize( size );
}

char* SetGet::buf()
{
	return &buf_[0];
}

/////////////////////////////////////////////////////////////////////////

/**
 * Here we generate the actual opfunc names from the field name.
 * We use the setFid only to do type checking. The actual Fid we
 * need to call is the getFid, which is passed back.
 */
bool SetGet::checkGet( const string& field, FuncId& getFid )	
	const
{
	static const SetGet1< FuncId > sgf( shelle_ );

	string setField = "set_" + field;
	string getField = "get_" + field;
	FuncId setFid = e_.element()->cinfo()->getOpFuncId( setField );
	getFid = e_.element()->cinfo()->getOpFuncId( getField );
	const OpFunc* setFunc = e_.element()->cinfo()->getOpFunc( setFid );
	const OpFunc* getFunc = e_.element()->cinfo()->getOpFunc( getFid );
	if ( !( setFunc && getFunc ) ) {
		cout << "get::Failed to find " << e_ << "." << field << endl;
		return 0;
	}
	if ( !getFunc->checkSet( &sgf ) ) {
		 cout << "get::Type mismatch on getFunc" << e_ << "." << field << endl;
		return 0;
	}
	if ( !setFunc->checkSet( this ) ) {
		 cout << "get::Type mismatch on return value" << e_ << "." << field << endl;
		return 0;
	}
	return 1;
}

bool SetGet::iGet( const string& field ) const
{
	static ConnId getCid = 0;
	static unsigned int getFuncIndex = 0;
	static FuncId retFunc = shell_->cinfo()->getOpFuncId( "handleGet" );

	FuncId destFid;
	if ( checkGet( field, destFid ) ) {
		shell_->clearConn( getCid );
		Msg* m = new SingleMsg( shelle_, e_ );
		shell_->addMsgToConn( m, getCid );
		shell_->addTargetFunc( destFid, getFuncIndex );
		shelle_.asend( getCid, getFuncIndex, 
			reinterpret_cast< char* >( &retFunc ), sizeof( FuncId ) );
		return 1;
	}
	return 0;
}
