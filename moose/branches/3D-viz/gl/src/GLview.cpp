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

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Constants.h"
#include "AckPickData.h"

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
	sockFd_( -1 ),
	isConnectionUp_( false ),
	strClientHost_( "127.0.0.1" ),
	strClientPort_( "" ),
	syncMode_( false ),
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
	
	double maxsize = populateXYZ(); // this will be the maximum size (absolute value) of
					// our elements along any dimension

	


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

int GLview::getXYZ( Id id, double& x, double& y, double& z, double &maxsize )
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
			return getXYZ( parent, x, y, z, maxsize ); // recurses
	}
	
	// success
	get< double >( id.eref(), "x", x );
	get< double >( id.eref(), "y", y );
	get< double >( id.eref(), "z", z );
	
	double maxsize_ = 0, temp;
	if ( id.eref().e->findFinfo( "diameter") &&
	     get< double >( id.eref(), "diameter", temp ) &&
	     temp > maxsize_)
	{
		maxsize_ = temp;
	}
	if ( id.eref().e->findFinfo( "length") &&
	     get< double >( id.eref(), "length", temp ) &&
	     temp > maxsize_)
	{
		maxsize_ = temp;
	}
	maxsize = maxsize_;

	return 0;
}

double GLview::populateXYZ()
{
	if ( x_ == NULL )
		x_ = ( double * ) malloc( sizeof( double ) * elements_.size() );
	if ( y_ == NULL )
		y_ = ( double * ) malloc( sizeof( double ) * elements_.size() );
	if ( z_ == NULL )
		z_ = ( double * ) malloc( sizeof( double ) * elements_.size() );

	double x, y, z, size, maxsize = 0;

	// There are three passes for determining collision-free x,y,z
	// co-ordinates (to 6 decimal places). This procedure also
	// determines the maximum length in any dimension of elements
	// with given geometries so that the remaining elements (with
	// no geometrical basis) can be assigned sizes on the same
	// scale of size; this is based on the assumption that most
	// geometrical elements in the simulation will be laid out to
	// be non-overlapping and therefore the maximum length
	// provides an approximation of the typical distance between
	// elements.

	// 1. We get x,y,z from elements or their first non-root
	// ancestor that have valid x,y,z values. We check these into
	// a map, intending to separate elements specified with
	// duplicate x,y,z co-ordinates.
	map< string, unsigned int > mapXYZ;

	for ( unsigned int i = 0; i < elements_.size(); ++i )
	{
		if ( getXYZ( elements_[i], x, y, z, size ) == 0 )
		{
			string key = boxXYZ( x, y, z );
			if ( mapXYZ.count( key ) == 0 )
				mapXYZ[ key ] = 1;
			else
				mapXYZ[ key ] += 1;

			if ( size > maxsize )
				maxsize = size;
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
		if ( getXYZ( elements_[i], x, y, z, size ) == 0 )
		{
			string key = boxXYZ( x, y, z );
			if ( mapXYZ[key] > 1 ) // collision
			{
				unassignedElements.push_back( i );
			}
			else
			{
				x_[i] = x;
				y_[i] = y;
				z_[i] = z;
				
				if ( bbx < x )
					bbx = x + maxsize;
				if ( bby < y )
					bby = y + maxsize;
				if ( bbz < z )
					bbz = z + maxsize;
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

	// Another heuristic: if maxsize is still zero, i.e., no
	// non-root ancestors with valid geometries were found, we
	// must still set the size to an arbitrary non-zero value, so
	// we use 1.
	if ( maxsize < FP_EPSILON )
		maxsize = 1;

	for ( unsigned int j = 0; j < unassignedElements.size(); ++j )
	{
		unsigned int i = unassignedElements[j];
		
		x_[i] = bbx + (j % n) * maxsize;
		y_[i] = bby + (j / n) * maxsize;
		z_[i] = bbz;
	}

	return maxsize;
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

void* GLview::getInAddress( struct sockaddr *sa )
{
	if ( sa->sa_family == AF_INET ) {
		return &( ( ( struct sockaddr_in* )sa )->sin_addr );
	}

	return &( ( ( struct sockaddr_in6* )sa )->sin6_addr );
}

int GLview::getSocket( const char* hostname, const char* service )
{
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	
	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( ( rv = getaddrinfo( hostname, service, &hints, &servinfo ) ) != 0 ) {
		std::cerr << "GLview error: getaddrinfo: " << gai_strerror( rv ) << std::endl;
		return -1;
	}
	// loop through all the results and connect to the first we can
	for ( p = servinfo; p != NULL; p = p->ai_next ) {
		if ( ( sockFd_ = socket( p->ai_family, p->ai_socktype,
				     p->ai_protocol ) ) == -1 ) {
		    //std::cerr << "GLview error: socket" << std::endl;
			continue;
		}
		
		if ( connect( sockFd_, p->ai_addr, p->ai_addrlen ) == -1 ) {
			close( sockFd_ );
			//std::cerr << "GLview error: connect" << std::endl;
			continue;
		}
		
		break;
	}

	if ( p == NULL ) {
		std::cerr << "GLcell error: failed to connect" << std::endl;
		return -1;
	}
	
	inet_ntop( p->ai_family, getInAddress( ( struct sockaddr * )p->ai_addr ),
		   s, sizeof s );
	// std::cout << "Connecting to " << s << std::endl;
	
	freeaddrinfo( servinfo );
	
	isConnectionUp_ = true;
	return sockFd_;
}

int GLview::sendAll( int socket, char* buf, int* len )
{
	int total = 0;        // how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n;

	while ( total < *len )
	{
		n = send( socket, buf+total, bytesleft, 0 );
		if ( n == -1 )
		{
			std::cerr << "GLview error: send error; errno: " << errno << " " << strerror( errno ) << std::endl;    
			break;
		}
		total += n;
		bytesleft -= n;
	}

	*len = total; // return number actually sent here

	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

int GLview::recvAll( int socket, char* buf, int* len)
{
	int total = 0;        // how many bytes we've received
	int bytesleft = *len; // how many we have left to receive
	int n;
	
	while ( total < *len )
	{
		n = recv( socket, buf+total, bytesleft, 0 );
		if ( n == -1 )
		{
			std::cerr << "GLview error: recv error; errno: " << errno << " " << strerror( errno ) << std::endl;
			break;
		}
		total += n;
		bytesleft -= n;
	}
	
	*len = total; /// return number actually received here
	
	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

int GLview::receiveAck()
{
	if ( ! isConnectionUp_ )
	{
		std::cerr << "Could not receive ACK because the connection is down." << std::endl;
		return -1;
	}

	int numBytes, inboundDataSize;
	char header[MSGSIZE_HEADERLENGTH + 1];
	char *buf;

	numBytes = sizeof( AckPickData ) + 1;
	buf = ( char * ) malloc( numBytes );

	numBytes = MSGSIZE_HEADERLENGTH + 1;
	if ( recvAll( sockFd_, header, &numBytes) == -1 ||
	     numBytes < MSGSIZE_HEADERLENGTH + 1 )
	{
		std::cerr << "GLview error: could not receive Ack header!" << std::endl;
		return -1;
	}
	else
	{
		std::istringstream ackHeaderStream( std::string( header,
								 MSGSIZE_HEADERLENGTH ) );
		ackHeaderStream >> std::hex >> inboundDataSize;
	}

	numBytes = inboundDataSize + 1;
	buf = ( char * ) malloc( numBytes * sizeof( char ) );

	if ( recvAll( sockFd_, buf, &numBytes ) == -1 ||
	     numBytes < inboundDataSize + 1 )
	{
		std::cerr << "GLview error: could not receive Ack!" << std::endl;
		return -2;
	}
	else
	{
		std::istringstream archiveStream( std::string( buf, inboundDataSize ) );

		// starting new scope so that the archive's stream's destructor is called after the archive's
		{
			boost::archive::text_iarchive archive( archiveStream );
		
			AckPickData ackPickData;
		
			archive >> ackPickData;

			if ( ackPickData.wasSomethingPicked )
			{
				handlePick( ackPickData.idPicked );
			}
		}
	}

	free(buf);

	return 1;
}

void GLview::handlePick( unsigned int idPicked )
{
	std::cout << "Compartment with id " << idPicked << " was picked!" << std::endl;
}

void GLview::disconnect()
{
	if ( ! isConnectionUp_ )
	{
		sockFd_ = getSocket( strClientHost_.c_str(), strClientPort_.c_str() );
		if ( sockFd_ == -1 ) 
		{
			std::cerr << "GLview error: couldn't connect to client!" << std::endl;
			return;
		}
	}

	std::ostringstream headerStream;
	headerStream << std::setw( MSGSIZE_HEADERLENGTH ) << 0;
	headerStream << std::setw( MSGTYPE_HEADERLENGTH ) << DISCONNECT;

	int headerLen = headerStream.str().size() + 1;
	char* headerData = (char *) malloc( headerLen * sizeof( char ) );
	strcpy( headerData, headerStream.str().c_str() );


	if ( sendAll( sockFd_, headerData, &headerLen ) == -1 ||
	     headerLen < headerStream.str().size() + 1 )
	{
		std::cerr << "GLview error: couldn't transmit DISCONNECT message to client!" << std::endl;
	}

	free( headerData );
	close( sockFd_ );
}

template< class T >
void GLview::transmit( T& data, MSGTYPE messageType )
{
	if ( strClientHost_.empty() || strClientPort_.empty() ) // these should have been set.
		return;

	if ( ! isConnectionUp_ )
	{
		sockFd_ = getSocket( strClientHost_.c_str(), strClientPort_.c_str() );
		if ( sockFd_ == -1 ) 
		{
			std::cerr << "GLcell error: Couldn't connect to client!" << std::endl;
			return;
		}
	}

	std::ostringstream archiveStream;

	// starting new scope so that the archive's stream's destructor is called after the archive's
	{
		boost::archive::text_oarchive archive( archiveStream );

		archive << data;

		std::ostringstream headerStream;
		headerStream << std::setw( MSGSIZE_HEADERLENGTH )
			     << std::hex << archiveStream.str().size();

		headerStream << std::setw( MSGTYPE_HEADERLENGTH )
			     << messageType;

		int headerLen = headerStream.str().size() + 1;
		char* headerData = ( char * ) malloc( headerLen * sizeof( char ) );
		strcpy( headerData, headerStream.str().c_str() );
	
		if ( sendAll( sockFd_, headerData, &headerLen ) == -1 ||
		     headerLen < headerStream.str().size() + 1 )
		{
			std::cerr << "GLcell error: couldn't transmit header to client!" << std::endl;

			isConnectionUp_ = false;
			close( sockFd_ );
		}
		else
		{
			int archiveLen = archiveStream.str().size() + 1;
			char* archiveData = ( char * ) malloc( archiveLen * sizeof( char ) );
			strcpy( archiveData, archiveStream.str().c_str() );
				
			if ( sendAll( sockFd_, archiveData, &archiveLen ) == -1 ||
			     archiveLen < archiveStream.str().size() + 1 )
			{
				std::cerr << "GLcell error: couldn't transmit data to client!" << std::endl;	
			}
			free( archiveData );
		}
		free( headerData );
	}
}
