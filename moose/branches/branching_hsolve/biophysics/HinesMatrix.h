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
	JunctionStruct( int i, int r ) :
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
		const vector< vector< unsigned int > >& tree,
		const vector< double >& Ga );

protected:
	vector< JunctionStruct >  junction_;
	vector< double >          HS_;
	vector< double >          HJ_;
	vector< double >          HJcopy_;
	vector< double >          VMid_;
	vector< double* >         operand_;
	vector< double* >         backOperand_;

private:
	makeJunctions( );
	makeMatrix( );
	makeOperands( );

	vector< vector< unsigned int > >*  tree_;
	vector< double >*                  Ga_;
	vector< vector< unsigned int > >   junctionGroup_;
	vector< double* >                  operandBase_;
	map< unsigned int, unsigned int >  groupNumber_;
};

#endif // _HINES_MATRIX_H

