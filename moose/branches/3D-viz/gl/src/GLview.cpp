/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include "shell/Shell.h"
#include "element/Wildcard.h"

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
		new ValueFinfo( "relpath",
				ValueFtype1< string >::global(),
				GFCAST( &GLview::getRelPath ),
				RFCAST( &GLview::setRelPath )
				),
		new ValueFinfo( "value1",
				ValueFtype1< string >::global(),
				GFCAST( &GLview::getValue1Field ),
				RFCAST( &GLview::setValue1Field )
				),
		new ValueFinfo( "value2",
				ValueFtype1< string >::global(),
				GFCAST( &GLview::getValue2Field ),
				RFCAST( &GLview::setValue2Field )
				),
		new ValueFinfo( "value3",
				ValueFtype1< string >::global(),
				GFCAST( &GLview::getValue3Field ),
				RFCAST( &GLview::setValue3Field )
				),
		new ValueFinfo( "value4",
				ValueFtype1< string >::global(),
				GFCAST( &GLview::getValue4Field ),
				RFCAST( &GLview::setValue4Field )
				),
		new ValueFinfo( "value5",
				ValueFtype1< string >::global(),
				GFCAST( &GLview::getValue5Field ),
				RFCAST( &GLview::setValue5Field )
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
	strPath_( "" ),
	strRelPath_( "" ),
	values1_( NULL ),
	values2_( NULL ),
	values3_( NULL ),
	values4_( NULL ),
	values5_( NULL ),
	strValue1Field_( "" ),
	strValue2Field_( "" ),
	strValue3Field_( "" ),
	strValue4Field_( "" ),
	strValue5Field_( "" ),
	x_( NULL ),
	y_( NULL ),
	z_( NULL )
{
}

GLview::~GLview()
{
	// disconnect(); // TODO karan
	
	if ( values1_ != NULL )
		free( values1_ );
	if ( values2_ != NULL )
		free( values2_ );
	if ( values3_ != NULL )
		free( values3_ );
	if ( values4_ != NULL )
		free( values4_ );
	if ( values5_ != NULL )
		free( values5_ );
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
	return static_cast< const GLview * >( e.data() )->strPath_;
}

void GLview::setRelPath( const Conn* c, string strRelPath )
{
	static_cast< GLview * >( c->data() )->innerSetRelPath( strRelPath );
}

void GLview::innerSetRelPath( const string& strRelPath )
{
	strRelPath_ = strRelPath;
}

string GLview::getRelPath( Eref e )
{
	return static_cast< GLview * >( e.data() )->strRelPath_;
}

void GLview::setValue1Field( const Conn* c, string strValue1Field )
{
	static_cast< GLview * >( c->data() )->innerSetValue1Field( strValue1Field );
}

void GLview::innerSetValue1Field( const string& strValue1Field )
{
	strValue1Field_ = strValue1Field;
}

string GLview::getValue1Field( Eref e )
{
	return static_cast< GLview * >( e.data() )->strValue1Field_;
}

void GLview::setValue2Field( const Conn* c, string strValue2Field )
{
	static_cast< GLview * >( c->data() )->innerSetValue2Field( strValue2Field );
}

void GLview::innerSetValue2Field( const string& strValue2Field )
{
	strValue2Field_ = strValue2Field;
}

string GLview::getValue2Field( Eref e )
{
	return static_cast< GLview * >( e.data() )->strValue2Field_;
}

void GLview::setValue3Field( const Conn* c, string strValue3Field )
{
	static_cast< GLview * >( c->data() )->innerSetValue3Field( strValue3Field );
}

void GLview::innerSetValue3Field( const string& strValue3Field )
{
	strValue3Field_ = strValue3Field;
}

string GLview::getValue3Field( Eref e )
{
	return static_cast< GLview * >( e.data() )->strValue3Field_;
}

void GLview::setValue4Field( const Conn* c, string strValue4Field )
{
	static_cast< GLview * >( c->data() )->innerSetValue4Field( strValue4Field );
}

void GLview::innerSetValue4Field( const string& strValue4Field )
{
	strValue4Field_ = strValue4Field;
}

string GLview::getValue4Field( Eref e )
{
	return static_cast< GLview * >( e.data() )->strValue4Field_;
}

void GLview::setValue5Field( const Conn* c, string strValue5Field )
{
	static_cast< GLview * >( c->data() )->innerSetValue5Field( strValue5Field );
}

void GLview::innerSetValue5Field( const string& strValue5Field )
{
	strValue5Field_ = strValue5Field;
}

string GLview::getValue5Field( Eref e )
{
	return static_cast< GLview * >( e.data() )->strValue5Field_;
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
	elements_.clear();

	wildcardFind( strPath_, elements_ );
	std::cout << "GLview: " << elements_.size() << " elements found." << std::endl; // TODO comment this out by default

	if ( ! strValue1Field_.empty() )
		populateValues( 1, values1_, strValue1Field_ ); 
	if ( ! strValue2Field_.empty() )
		populateValues( 2, values2_, strValue2Field_ ); 
	if ( ! strValue3Field_.empty() )
		populateValues( 3, values3_, strValue3Field_ ); 
	if ( ! strValue4Field_.empty() )
		populateValues( 4, values4_, strValue4Field_ ); 
	if ( ! strValue5Field_.empty() )
		populateValues( 5, values5_, strValue5Field_ ); 
	
	// TODO must get x,y,z in reinitFuncLocal
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

int GLview::populateValues( int valueNum, double * values, string strValueField )
{
	int status = 0;

	if ( values == NULL)
		values = ( double * ) malloc( sizeof( double ) * elements_.size() );

	if ( values == NULL )
	{
		std::cerr << "GLview error: could not allocate memory to set field values" << std::endl;
		status = -1;
	}
	else
	{
		for ( unsigned int i = 0; i < elements_.size(); ++i)
		{
			Id id = elements_[i];
			string path;

			if ( ! strRelPath_.empty() ) 
			{
				path = elements_[i].path();
				path.push_back('/');
				path.append(strRelPath_);
				id = Id::Id( path, "/" );
			}
			
			if ( id.eref() == NULL )
			{
				std::cerr << "Could not find path: " << path << "; error in relpath? " << std::endl;
				status = -2;
				break;
			}
			else if ( ! get< double >( id.eref(), strValueField, values[i] ) )
			{
				std::cerr << "GLview error: for value" << valueNum << ", unable to find a field called '" << strValueField << "' in elements on the path " << strPath_ << strRelPath_ << std::endl;
				status = -3;
				break;
			}
			/* else
			   {
			   std::cout << "field1 value " << strValueField << " for element " << i << " is " << values[i] << std::endl;
			   } */
		}
	}

	if ( status == -2 || status == -3 )
		free( values );

	return status;
}

///////////////////////////////////////////////////
// networking helper function definitions
///////////////////////////////////////////////////

