/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSolveModelMus_h
#define _HSolveModelMus_h

class HSolveModelMus: public HSolveModel < Element* >
{
public:
	void getFieldInt( Element* const& object,
		const string& field,
		int& value ) const;

	void getFieldDouble( Element* const& object,
		const string& field,
		double& value ) const;

	double getAlpha( Element* const& gate, double V ) const;

	double getBeta( Element* const& gate, double V ) const;

	vector< Element* > neighbours( Element* const& compartment) const;

	vector< Element* > channels( Element* const& compartment ) const;

	vector< Element* > gates( Element* const& channel ) const;
};

#endif
