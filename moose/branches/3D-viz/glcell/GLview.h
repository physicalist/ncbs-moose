/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

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

	static const int MSGTYPE_HEADERLENGTH;
     	static const int MSGSIZE_HEADERLENGTH;
	static const char SYNCMODE_ACKCHAR;

 private:
	string strPath_;
};
