/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv.h>
#include "MarkovGsl.h"

///////////////////////////////////////////////////
// Basic class function definitions
///////////////////////////////////////////////////

MarkovGsl::MarkovGsl()
{
	isInitialized_ = 0;
	method_ = "rk5";
#ifdef USE_GSL
	gslStepType_ = gsl_odeiv_step_rkf45;
	gslStep_ = 0;
#endif // USE_GSL
	absAccuracy_ = 1.0e-6;
	relAccuracy_ = 1.0e-6;
	internalStepSize_ = 1.0e-4;
	gslEvolve_ = NULL;
	gslControl_ = NULL;
}

MarkovGsl::~MarkovGsl()
{
	if ( gslEvolve_ )
		gsl_odeiv_evolve_free( gslEvolve_ );
	if ( gslControl_ )
		gsl_odeiv_control_free( gslControl_ );
	if ( gslStep_ )
		gsl_odeiv_step_free( gslStep_ );
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

bool MarkovGsl::getIsInitialized() const
{
	return isInitialized_;
}

string MarkovGsl::getMethod() const
{
	return method_;
}

void MarkovGsl::setMethod( string method )
{
#ifdef USE_GSL
	method_ = method;
	gslStepType_ = 0;
	// cout << "in void MarkovGsl::innerSetMethod( string method ) \n";
	if ( method == "rk2" ) {
		gslStepType_ = gsl_odeiv_step_rk2;
	} else if ( method == "rk4" ) {
		gslStepType_ = gsl_odeiv_step_rk4;
	} else if ( method == "rk5" ) {
		gslStepType_ = gsl_odeiv_step_rkf45;
	} else if ( method == "rkck" ) {
		gslStepType_ = gsl_odeiv_step_rkck;
	} else if ( method == "rk8pd" ) {
		gslStepType_ = gsl_odeiv_step_rk8pd;
	} else if ( method == "rk2imp" ) {
		gslStepType_ = gsl_odeiv_step_rk2imp;
	} else if ( method == "rk4imp" ) {
		gslStepType_ = gsl_odeiv_step_rk4imp;
	} else if ( method == "bsimp" ) {
		gslStepType_ = gsl_odeiv_step_rk4imp;
		cout << "Warning: implicit Bulirsch-Stoer method not yet implemented: needs Jacobian\n";
	} else if ( method == "gear1" ) {
		gslStepType_ = gsl_odeiv_step_gear1;
	} else if ( method == "gear2" ) {
		gslStepType_ = gsl_odeiv_step_gear2;
	} else {
		cout << "Warning: MarkovGsl::innerSetMethod: method '" <<
			method << "' not known, using rk5\n";
		gslStepType_ = gsl_odeiv_step_rkf45;
	}
#endif // USE_GSL
}

double MarkovGsl::getRelativeAccuracy() const
{
	return relAccuracy_;
}
void MarkovGsl::setRelativeAccuracy( double value )
{
	relAccuracy_ = value;
}

double MarkovGsl::getAbsoluteAccuracy() const
{
	return absAccuracy_;
}
void MarkovGsl::setAbsoluteAccuracy( double value )
{
	absAccuracy_ = value;
}

double MarkovGsl::getInternalDt() const
{
	return internalStepSize_;
}

void MarkovGsl::setInternalDt( double value )
{
	internalStepSize_ = value;
}

double* MarkovGsl::solve( double currTime, double dt, double* state, unsigned int numStates ) 
{
	double nextt = currTime + dt;
	double t = currTime, sum = 0;

	while ( t < nextt ) {
		int status = gsl_odeiv_evolve_apply ( 
			gslEvolve_, gslControl_, gslStep_, &gslSys_, 
			&t, nextt,
			&internalStepSize_, state);

		//Simple idea borrowed from Dieter Jaeger's implementation of a Markov
		//channel to deal with potential round-off error.
		sum = 0;
		for ( unsigned int i = 0; i < numStates; i++ )
			sum += state[i];

		for ( unsigned int i = 0; i < numStates; i++ )
			state[i] /= sum;

		if ( status != GSL_SUCCESS )
			break;
	}

	return state;
}
