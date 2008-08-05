/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSOLVE_PASSIVE_H
#define _HSOLVE_PASSIVE_H

struct CompartmentStruct
{
	CompartmentStruct( double c, double e, double i ) :
		CmByDt( c ),
		EmByRm( e ),
		inject( i )
	{ ; }

	double CmByDt;
	double EmByRm;
	double inject;
};

class HSolvePassive: public HinesMatrix
{
public:
	void setup( Id seed );
	void solve( );

protected:
	double                       dt_;
	unsigned int                 nCompt_;
	vector< CompartmentStruct >  compartment_;
	vector< Id >                 compartmentId_;
	vector< double >             V_;

private:
	// Setting up of data structures
	void walkTree( seed );
	void initialize( );
	void storeTree( );
	
	// Integration
	void forwardEliminate( );
	void backwardSubstitute( );
	
	vector< vector< int > >  tree_;
	vector< double >         Ga_;
};

#endif // _HSOLVE_PASSIVE_H

