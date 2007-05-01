/*/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "ZGate.h"
#include <cmath>

double ZGate::alpha( double V ) {
	switch ( alphaForm_ ) {
	case 1:	// EXPONENTIAL
		return ( alpha_A_ * ( exp( ( V - alpha_V0_ ) / alpha_B_ ) ) );
	case 2:	// SIGMOID
		return ( alpha_A_ / (1 + exp( ( V - alpha_V0_ ) / alpha_B_ ) ) );
	case 3:	// LINOID
		return ( alpha_A_ * ( V - alpha_V0_ ) ) / ( exp( ( V - alpha_V0_ ) / alpha_B_ ) - 1 );
	default:
		return 0.0;
	}
}

double ZGate::beta( double V ) {
	switch ( betaForm_ ) {
	case 1:	// EXPONENTIAL
		return ( beta_A_ * ( exp( ( V - beta_V0_ ) / beta_B_ ) ) );
	case 2:	// SIGMOID
		return ( beta_A_ / ( 1 + exp( ( V - beta_V0_ ) / beta_B_ ) ) );
	case 3:	// LINOID
		return ( beta_A_ * ( V - beta_V0_ ) ) / ( exp( ( V - beta_V0_ ) / beta_B_ ) - 1 );
	default:
		return 0.0;
	}
}
