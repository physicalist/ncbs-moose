/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include "RecvFunc.h"
#include "Ftype.h"
#include "FuncVec.h"

using namespace std;

static const unsigned int EMPTY_ID = 0;

/**
 * This function manages a static vector of FuncVecs.
 */
static vector< FuncVec* >& funcVecLookup()
{
	static vector< FuncVec* > fv;

	return fv;
}

FuncVec::FuncVec( const string& className, const string& finfoName )
{
	name_ = className + "." + finfoName;
	funcVecLookup().push_back( this );
}


/// addFunc pushes a new function onto the FuncVec.
void FuncVec::addFunc( RecvFunc func, const Ftype* ftype )
{
	func_.push_back( func );
	funcType_.push_back( ftype );
	// parFuncSync_.push_back( ftype->parFuncSync() );
	// parFuncAsync_.push_back( ftype->parFuncAsync() );
}

/// func returns the indexed function.
RecvFunc FuncVec::func( unsigned int funcNum ) const
{
	assert( funcNum < func_.size() );
	return func_[ funcNum ];
}

/**
* parFuncSync returns a destination function that will handle
* the identical arguments and package them for sending to a
* remote node on a parallel message call.
* The Sync means that this func is for synchronous data.
*/
RecvFunc FuncVec::parFuncSync( unsigned int funcNum ) const
{
	assert( funcNum < func_.size() );
	return parFuncSync_[ funcNum ];
}

/**
* parFuncAsync returns a destination function that will handle
* the identical arguments and package them for sending to a
* remote node on a parallel message call.
* The Async means that this func is for synchronous data.
*/
RecvFunc FuncVec::parFuncAsync( unsigned int funcNum ) const
{
	assert( funcNum < func_.size() );
	return parFuncAsync_[ funcNum ];
}

/**
* fType returns a vector of Ftypes for the functions
*/
const vector< const Ftype* >& FuncVec::fType() const
{
	return funcType_;
}

/**
 * This function returns the FuncVec belonging to the specified id
 */
const FuncVec* FuncVec::getFuncVec( unsigned int id )
{
	assert( funcVecLookup().size() > id );

	return funcVecLookup()[id];
}

const string& FuncVec::name() const
{
	return name_;
}

static bool fvcmp( const FuncVec* a, const FuncVec* b )
{
	return ( a->name() < b->name() );
}
/**
 * sortFuncVec puts them in order and assigns ids.
 * Must be called before any messaging is begun, because we'll need
 * the FuncVecs for that.
 */
void FuncVec::sortFuncVec( )
{
	vector< FuncVec* >& fv = funcVecLookup();
	sort( fv.begin(), fv.end(), fvcmp );
	FuncVec* empty = new FuncVec( "empty", "empty" );
	fv.insert( fv.begin(), empty );
	// Note that 'empty' is at zero.
	for ( unsigned int i = 0; i < fv.size(); i++ ) {
		if ( fv[i]->size() == 0 )
			fv[i]->id_ = EMPTY_ID;
		else
			fv[i]->id_ = i;
	}
	cout << fv.size() << " FuncVecs built.\n";
}

/**
* This static identifies a FuncVec without entries.
*/
unsigned int FuncVec::emptyId()
{
	return EMPTY_ID;
}
