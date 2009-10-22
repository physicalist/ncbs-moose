/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include "../shell/Shell.h"
#include "GLview.h"

#include <iostream>
#include <string>

const int GLview::MSGTYPE_HEADERLENGTH = 1;
const int GLview::MSGSIZE_HEADERLENGTH = 8;
const char GLview::SYNCMODE_ACKCHAR = '*';

const Cinfo* initGLviewCinfo()
{
	static Finfo* processShared[] = 
	{
		new DestFinfo( "process", Ftype1< ProcInfo >::global(),
			RFCAST( &GLview::processFunc ) ),
		new DestFinfo( "reinit", Ftype1< ProcInfo >::global(),
			RFCAST( &GLview::reinitFunc ) ),
	};
	static Finfo* process = new SharedFinfo( "process", processShared,
		sizeof( processShared ) / sizeof( Finfo* ),
		"shared message to receive Process messages from scheduler objects" );

	static Finfo* GLviewFinfos[] = 
	{
	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
		new ValueFinfo( "path",
				ValueFtype1< string >::global(),
				GFCAST( &GLview::getPath ),
				RFCAST( &GLview::setPath )
				),
	///////////////////////////////////////////////////////
	// Shared definitions
	///////////////////////////////////////////////////////
		//		parser,
		process,
	};

	// Schedule molecules for the slower clock, stage 0.
	static SchedInfo schedInfo[] = { { process, 0, 0 } };
	
	static string doc[] = 
	{
		"Name", "GLview",
		"Author", "Karan Vasudeva, 2009, NCBS",
		"Description", "GLview: class to drive the spatial map widget",
	};

	static Cinfo glviewCinfo(
				 doc,
				 sizeof( doc ) / sizeof( string ),
				 initNeutralCinfo(),
				 GLviewFinfos,
				 sizeof( GLviewFinfos ) / sizeof( Finfo * ),
				 ValueFtype1< GLview >::global(),
				 schedInfo, 1
				 );

	return &glviewCinfo;
}

static const Cinfo* glviewCinfo = initGLviewCinfo();

///////////////////////////////////////////////////
// Class function definitions
///////////////////////////////////////////////////

GLview::GLview()
	:
	strPath_( "" )
{
}
GLview::~GLview()
{
  // disconnect(); // TODO karan
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void GLview::setPath( const Conn* c, string strPath )
{
	static_cast< GLview * >( c->data() )->innerSetPath( strPath );
}

void GLview::innerSetPath( const string& strPath )
{
	strPath_ = strPath;
}

string GLview::getPath( Eref e )
{
	return static_cast< const GLview* >( e.data() )->strPath_;
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void GLview::reinitFunc( const Conn* c, ProcInfo info )
{
	static_cast< GLview * >( c->data() )->reinitFuncLocal( c );
}

void GLview::reinitFuncLocal( const Conn* c )
{
	std::cout << "in GLview reinit" << std::endl;
}

void GLview::processFunc( const Conn* c, ProcInfo info )
{
	static_cast< GLview * >( c->data() )->processFuncLocal( c->target(), info );
}

void GLview::processFuncLocal( Eref e, ProcInfo info )
{
	std::cout << "in GLview process" << std::endl;
}

///////////////////////////////////////////////////
// private function definitions
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// networking helper function definitions
///////////////////////////////////////////////////

