/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/*
#include <vector>
#include <string>
using namespace std;
*/
#include "header.h"
#include "MathFunc.h"
#include "FuncTerm.h"
#include "MathFuncTerm.h"


double MathTerm::operator() ( const double* S, double t ) const
{
	vector< double > args;
	for( vector< unsigned int >::const_iterator i = args_.begin(); 
		i != args_.end(); i++ )
		args.push_back( S[ *i ] );
	return func_->op( args );
}

unsigned int MathTerm::getReactants( vector< unsigned int >& molIndex )
	const
{
	molIndex = args_;
	return args_.size();
}

const string& MathTerm::function() const
{
	static string ret;
	ret = func_->getFunction();
	return ret;
}


double MathTimeTerm::operator() ( const double* S, double t ) const
{
	vector< double > args;
	args.push_back( t ); // First arg is t.
	for( vector< unsigned int >::const_iterator i = args_.begin(); 
		i != args_.end(); i++ )
		args.push_back( S[ *i ] );
	return func_->op( args );
}

unsigned int MathTimeTerm::getReactants( vector< unsigned int >& molIndex )
	const
{
	molIndex = args_;
	return args_.size();
}

const string& MathTimeTerm::function() const
{
	static string ret;
	ret = func_->getFunction();
	return ret;
}
