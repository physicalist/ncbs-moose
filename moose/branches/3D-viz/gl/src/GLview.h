/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"

enum MSGTYPE
{
	RESET,
	PROCESS,
	PROCESSSYNC,
	DISCONNECT
};

class GLview
{
 public:
	GLview();
	~GLview();

	static void process( const Conn* c, ProcInfo p );

	static void processFunc( const Conn* c, ProcInfo p );
	void processFuncLocal( Eref e, ProcInfo info );

	static void reinitFunc( const Conn* c, ProcInfo info );
	void reinitFuncLocal( const Conn* c );

	static void setPath( const Conn* c, string strPath );
	void innerSetPath( const string& strPath );
	static string getPath( Eref e );
	
	static void setRelPath( const Conn* c, string strRelPath );
	void innerSetRelPath( const string& strRelPath );
	static string getRelPath( Eref e );

	static void setValue1Field( const Conn* c, string strValue1Field );
	void innerSetValue1Field( const string& strValue1Field );
	static string getValue1Field( Eref e );

	static void setValue2Field( const Conn* c, string strValue2Field );
	void innerSetValue2Field( const string& strValue2Field );
	static string getValue2Field( Eref e );
	
	static void setValue3Field( const Conn* c, string strValue3Field );
	void innerSetValue3Field( const string& strValue3Field );
	static string getValue3Field( Eref e );
	
	static void setValue4Field( const Conn* c, string strValue4Field );
	void innerSetValue4Field( const string& strValue4Field );
	static string getValue4Field( Eref e );
	
	static void setValue5Field( const Conn* c, string strValue5Field );
	void innerSetValue5Field( const string& strValue5Field );
	static string getValue5Field( Eref e );
	
	static const int MSGTYPE_HEADERLENGTH;
     	static const int MSGSIZE_HEADERLENGTH;
	static const char SYNCMODE_ACKCHAR;

 private:
	string strPath_;
	string strRelPath_;

	vector< Id > elements_; // the elements found on strPath_

	double *values1_;
	double *values2_;
	double *values3_;
	double *values4_;
	double *values5_;
	string strValue1Field_;
	string strValue2Field_;
	string strValue3Field_;
	string strValue4Field_;
	string strValue5Field_;

	double *x_;
	double *y_;
	double *z_;

	int populateValues( int valueNum, double * values, string strValueField );
  
};
