/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "SetGet.h"
#include "ReduceBase.h"
#include "ReduceMax.h"
#include "../shell/Shell.h"

/*
Eref SetGet::shelle_( 0, 0 );
Element* SetGet::shell_;

void SetGet::setShell()
{
	Id shellid;
	shelle_ = shellid.eref();
	shell_ = shelle_.element();
}
*/

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
	// e_.element()->clearQ();
	// Qinfo::clearQ( Shell::procInfo() );
}

//////////////////////////////////////////////////////////////////////
// A group of functions to forward dispatch commands to the Shell.
//////////////////////////////////////////////////////////////////////

void SetGet::dispatchSet( const Eref& er, FuncId fid, 
	const char* args, unsigned int size )
{
	Shell::dispatchSet( er, fid, args, size );
}

void SetGet::dispatchSetVec( const Eref& er, FuncId fid, 
	const PrepackedBuffer& arg )
{
	Shell::dispatchSetVec( er, fid, arg );
}

const vector< char* >& SetGet::dispatchGet( 
	const Eref& er, const string& field,
	const SetGet* sg, unsigned int& numGetEntries )
{
	return Shell::dispatchGet( er, field, sg, numGetEntries );
}
//////////////////////////////////////////////////////////////////////

bool SetGet::checkSet( const string& field, Eref& tgt, FuncId& fid ) const
{
	// string field = "set_" + destField;
	const Finfo* f = e_.element()->cinfo()->findFinfo( field );
	if ( !f ) { // Could be a child element? Note that field name will 
		// change from set_<name> to just <name>
		string f2 = field.substr( 4 );
		Id child = Neutral::child( e_, f2 );
		if ( child == Id() ) {
			cout << "Error: SetGet:checkSet:: No field or child named '" <<
				field << "' was found\n";
		} else {
			f = child()->cinfo()->findFinfo( "set_this" );
			assert( f ); // should always work as Neutral has the field.
			if ( child()->dataHandler()->totalEntries() == 
				e_.element()->dataHandler()->totalEntries() ) {
				tgt = Eref( child(), e_.index() );
				if ( !tgt.isDataHere() )
					return 0;
			} else if ( child()->dataHandler()->totalEntries() <= 1 ) {
				tgt = Eref( child(), 0 );
				if ( !tgt.isDataHere() )
					return 0;
			} else {
				cout << "SetGet::checkSet: child index mismatch\n";
				return 0;
			}
		}
	} else {
		tgt = e_;
	}
	const DestFinfo* df = dynamic_cast< const DestFinfo* >( f );
	if ( !df )
		return 0;
	
	fid = df->getFid();
	const OpFunc* func = df->getOpFunc();

// 	fid = e_.element()->cinfo()->getOpFuncId( field );
//	const OpFunc* func = e_.element()->cinfo()->getOpFunc( fid );
	if ( !func ) {
		cout << "set::Failed to find " << e_ << "." << field << endl;
		return 0;
	}

	// This is the crux of the function: typecheck for the field.
	if ( func->checkSet( this ) ) {
		return 1;
	} else {
		cout << "set::Type mismatch" << e_ << "." << field << endl;
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////

// Static function
bool SetGet::strGet( const Eref& tgt, const string& field, string& ret )
{
	const Finfo* f = tgt.element()->cinfo()->findFinfo( field );
	if ( !f ) {
		cout << Shell::myNode() << ": Error: SetGet::strGet: Field " <<
			field << " not found on Element " << tgt.element()->getName() <<
			endl;
		return 0;
	}
	return f->strGet( tgt, field, ret );
}

bool SetGet::strSet( const Eref& tgt, const string& field, const string& v)
{
	const Finfo* f = tgt.element()->cinfo()->findFinfo( field );
	if ( !f ) {
		cout << Shell::myNode() << ": Error: SetGet::strSet: Field " <<
			field << " not found on Element " << tgt.element()->getName() <<
			endl;
		return 0;
	}
	return f->strSet( tgt, field, v );
}
