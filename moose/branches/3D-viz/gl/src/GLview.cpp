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
#include "element/Neutral.h"

#include "GLview.h"

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <limits>

#include <math.h>

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
	
	free ( values1_ );
	free ( values2_ );
	free ( values3_ );
	free ( values4_ );
	free ( values5_ );
	
	free ( x_ );
	free ( y_ );
	free ( z_ );
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
		populateValues( 1, &values1_, strValue1Field_ ); 
	if ( ! strValue2Field_.empty() )
		populateValues( 2, &values2_, strValue2Field_ ); 
	if ( ! strValue3Field_.empty() )
		populateValues( 3, &values3_, strValue3Field_ ); 
	if ( ! strValue4Field_.empty() )
		populateValues( 4, &values4_, strValue4Field_ ); 
	if ( ! strValue5Field_.empty() )
		populateValues( 5, &values5_, strValue5Field_ ); 
	
	populateXYZ();
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

int GLview::populateValues( int valueNum, double ** pValues, string strValueField )
{
	int status = 0;

	if ( *pValues == NULL )
		*pValues = ( double * ) malloc( sizeof( double ) * elements_.size() );
	
	if ( *pValues == NULL )
	{
		std::cerr << "GLview error: could not allocate memory to set field values" << std::endl;
		status = -1;
	}
	else
	{
		double * values = *pValues;

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
		free( *pValues );

	return status;
}

int GLview::getXYZ( Id id, double& x, double& y, double& z )
{
	if ( id() == Element::root() )
		return -1;

	if ( ! id.eref().e->findFinfo( "x" ) ||
	     ! id.eref().e->findFinfo( "y" ) ||
	     ! id.eref().e->findFinfo( "z" ) )
	{
		Id parent = Shell::parent( id );
		if ( parent == Id::badId() )
			return -1;
		else
			return getXYZ( parent, x, y, z ); // recurses
	}

	return 0;
}

void GLview::populateXYZ()
{
	if ( x_ == NULL )
		x_ = ( double * ) malloc( sizeof( double ) * elements_.size() );
	if ( y_ == NULL )
		y_ = ( double * ) malloc( sizeof( double ) * elements_.size() );
	if ( z_ == NULL )
		z_ = ( double * ) malloc( sizeof( double ) * elements_.size() );

	double x, y, z;

	// There are three passes for determining collision-free x,y,z
	// co-ordinates (to 6 decimal places):

	// 1. We get x,y,z from elements or their first non-root
	// ancestor that have valid x,y,z values. We check these into
	// a map, intending to separate elements specified with
	// duplicate x,y,z co-ordinates.
	map< string, unsigned int > mapXYZ;

	for ( unsigned int i = 0; i < elements_.size(); ++i )
	{
		if ( getXYZ( elements_[i], x, y, z ) == 0 )
		{
			string key = boxXYZ( x, y, z );
			if ( mapXYZ.count( key ) == 0 )
				mapXYZ[ key ] = 1;
			else
				mapXYZ[ key ] += 1;
		}
	}

	// 2. We finalize non-duplicate x,y,z co-ordinates and place
	// the rest on a list that will be automatically assigned sane
	// co-ordinates just outside the bounding box of the first
	// group. We also determine a corner of this bounding box to
	// act as the starting location of the second group.
	double bbx = 0;
	double bby = 0;
	double bbz = 0;
	vector< unsigned int > unassignedElements;

	for ( unsigned int i = 0; i < elements_.size(); ++i )
	{
		if ( getXYZ( elements_[i], x, y, z ) == 0 )
		{
			string key = boxXYZ( x, y, z );
			if ( mapXYZ[key] > 1 )
			{
				unassignedElements.push_back( i );
			}
			else
			{
				x_[i] = x;
				y_[i] = y;
				z_[i] = z;
				
				if ( bbx < x )
					bbx = x + 1; // TODO change this to max x which is defined by the macro MAX X or reassigned by a setfield
				if ( bby < y )
					bby = y + 1; // TODO see above
				if ( bbz < z )
					bbz = z + 1; // TODO see above
			}
		}
		else
		{
			unassignedElements.push_back( i );
		}
	}
	
	// 3. We take the starting location calculated in the last
	// step and assign co-ordinates to all elements on the
	// (collision/unassigned) list as described in the last
	// step. These co-ordinates will be assigned to lay out all
	// elements in a planar grid, approximating a square in shape.
	int n = int( sqrt( unassignedElements.size() ) );

	// TODO change 1s below to max_x, etc.
	for ( unsigned int j = 0; j < unassignedElements.size(); ++j )
	{
		unsigned int i = unassignedElements[j];
		
		x_[i] = bbx + (j % n) * 1;
		y_[i] = bby + (j / n) * 1;
		z_[i] = bbz;
	}
} 

string GLview::boxXYZ( const double& x, const double& y, const double& z )
{
	string key( inttostring( int( x * 1e6 ) ) );
	key.append( inttostring( int( y * 1e6 ) ) );
	key.append( inttostring( int( z * 1e6 ) ) );

	return key;
}

string GLview::inttostring( int i )
{
	std::string s;
	std::stringstream out;
	out << i;
	return out.str();
}

///////////////////////////////////////////////////
// networking helper function definitions
///////////////////////////////////////////////////

