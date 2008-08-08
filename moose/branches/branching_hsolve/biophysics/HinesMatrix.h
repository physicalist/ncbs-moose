/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HINES_MATRIX_H
#define _HINES_MATRIX_H

struct JunctionStruct
{
	JunctionStruct( unsigned int i, unsigned int r ) :
		index( i ),
		rank( r )
	{ ; }

	bool operator< ( const JunctionStruct& other ) const {
		return ( index < other.index );
	}

	unsigned int index;
	unsigned int rank;
};

class HinesMatrix
{
public:
	void setup(
		const vector< vector< unsigned int > >& children,
		const vector< double >& Ga,
		const vector< double >& CmByDt );
	
	double entry( unsigned int row, unsigned int col );

protected:
	unsigned int              nCompt_;
	vector< JunctionStruct >  junction_;
	vector< double >          HS_;
	vector< double >          HJ_;
	vector< double >          HJCopy_;
	vector< double >          VMid_;
	vector< double* >         operand_;
	vector< double* >         backOperand_;

private:
	void makeJunctions( );
	void makeMatrix( );
	void makeOperands( );

	vector< vector< unsigned int > > const  *children_;
	vector< double > const                  *Ga_;
	vector< double > const                  *CmByDt_;
	vector< vector< unsigned int > >         junctionGroup_;
	map< unsigned int, double* >             operandBase_;
	map< unsigned int, unsigned int >        groupNumber_;
};

#endif // _HINES_MATRIX_H
