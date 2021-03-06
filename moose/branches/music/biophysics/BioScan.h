/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _BIO_SCAN_H
#define _BIO_SCAN_H

class BioScan
{
public:
	static void initialize( Id object );
	static int adjacent( Id compartment, vector< Id >& ret );
	static int adjacent( Id compartment, Id exclude, vector< Id >& ret );
	static int children( Id compartment, vector< Id >& ret );
	static int channels( Id compartment, vector< Id >& ret );
	static int gates( Id channel, vector< Id >& ret );
	static int spikegen( Id compartment, vector< Id >& ret );
	static int synchan( Id compartment, vector< Id >& ret );
	static int caTarget( Id channel, vector< Id >& ret );
	static int caDepend( Id channel, vector< Id >& ret );
	static void rates(
		Id gate,
		const vector< double >& grid,
		vector< double >& A,
		vector< double >& B );
	static int modes(
		Id gate,
		int& AMode,
		int& BMode );

private:
	static int targets(
		Id object,
		const string& msg,
		vector< Id >& target,
		const string& type = "" );

	static bool isType( Id object, const string& type );
};

#endif // _BIO_SCAN_H
