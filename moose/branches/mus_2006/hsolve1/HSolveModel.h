/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSolveModel_h
#define _HSolveModel_h

template < class T >
class HSolveModel
{
public:
	vector< T > children( const T& self, const T& parent ) const {
		vector< T > child = neighbours( self );
		child.erase( remove( child.begin(), child.end(), parent ),
			child.end() );
		return child;
	}

	virtual vector< T > neighbours( const T& compartment ) const = 0;

	virtual vector< T > channels( const T& compartment ) const = 0;

	virtual vector< T > gates( const T& channel ) const = 0;

	virtual void getFieldInt( const T& object,
		const string& field,
		int& value ) const = 0;

	virtual void getFieldDouble( const T& object,
		const string& field,
		double& value ) const = 0;

	virtual double getAlpha( const T& gate, double V ) const = 0;

	virtual double getBeta( const T& gate, double V ) const = 0;

	virtual ~HSolveModel( ) {
		;
	}
};

#endif
