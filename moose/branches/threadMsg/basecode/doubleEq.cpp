/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#define EPS1 1e-6
#define EPS2 1e-12

#include <math.h>

bool doubleEq( double x, double y )
{
	double denom = fabs( x ) + fabs( y );
	if ( denom < EPS2 )
		denom = EPS2;
	return ( fabs( x - y ) / denom ) < EPS1;
}
