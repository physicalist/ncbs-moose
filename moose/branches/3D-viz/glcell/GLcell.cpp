/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

////////////////////////////////////////////////////////////////////////////////
// The socket code is mostly taken from Beej's Guide to Network Programming   //
// at http://beej.us/guide/bgnet/. The original code is in the public domain. //
////////////////////////////////////////////////////////////////////////////////

#include "moose.h"
#include "../shell/Shell.h"
#include "GLcell.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "GLcellCompartment.h"

const int GLcell::MSGTYPE_HEADERLENGTH = 1;
const int GLcell::MSGSIZE_HEADERLENGTH = 8;
const char GLcell::SYNCMODE_ACKCHAR = '*';

const Cinfo* initGLcellCinfo()
{
	static Finfo* processShared[] = 
	{
		new DestFinfo( "process", Ftype1< ProcInfo >::global(),
			RFCAST( &GLcell::processFunc ) ),
		new DestFinfo( "reinit", Ftype1< ProcInfo >::global(),
			RFCAST( &GLcell::reinitFunc ) ),
	};
	static Finfo* process = new SharedFinfo( "process", processShared,
		sizeof( processShared ) / sizeof( Finfo* ),
		"shared message to receive Process messages from scheduler objects" );

	static Finfo* GLcellFinfos[] = 
	{
	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
		new ValueFinfo( "path",
				ValueFtype1< string >::global(),
				GFCAST( &GLcell::getPath ),
				RFCAST( &GLcell::setPath )
				),
		new ValueFinfo( "host",
				ValueFtype1< string >::global(),
				GFCAST( &GLcell::getClientHost ),
				RFCAST( &GLcell::setClientHost )
				),
		new ValueFinfo( "port",
				ValueFtype1< string >::global(),
				GFCAST( &GLcell::getClientPort ),
				RFCAST( &GLcell::setClientPort )
				),
		new ValueFinfo( "attribute",
				ValueFtype1< string >::global(),
				GFCAST( &GLcell::getAttributeName ),
				RFCAST( &GLcell::setAttributeName )
				),
		new ValueFinfo( "threshold",
				ValueFtype1< double >::global(),
				GFCAST( &GLcell::getChangeThreshold ),
				RFCAST( &GLcell::setChangeThreshold )
				),
		new ValueFinfo( "sync",
				ValueFtype1< string >::global(),
				GFCAST( &GLcell::getSyncMode ),
				RFCAST( &GLcell::setSyncMode )
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
		"Name", "GLcell",
		"Author", "Karan Vasudeva, 2009, NCBS",
		"Description", "GLcell: class to drive the 3D cell visualization widget",
	};

	static Cinfo glcellCinfo(
				 doc,
				 sizeof( doc ) / sizeof( string ),
				 initNeutralCinfo(),
				 GLcellFinfos,
				 sizeof( GLcellFinfos ) / sizeof( Finfo * ),
				 ValueFtype1< GLcell >::global(),
				 schedInfo, 1
				 );

	return &glcellCinfo;
}

static const Cinfo* glcellCinfo = initGLcellCinfo();

///////////////////////////////////////////////////
// Class function definitions
///////////////////////////////////////////////////

GLcell::GLcell()
	:
	strPath_( "" ),
	strClientHost_( "127.0.0.1" ),
	strClientPort_( "" ),
	isConnectionUp_( false ),
	strAttributeName_( "Vm" ),
	sockFd_( -1 ),
	changeThreshold_( 1e-8 ),
	syncMode_( false )
{
}
GLcell::~GLcell()
{
	disconnect();
}

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////

void GLcell::setPath( const Conn* c, string strPath )
{
	static_cast< GLcell * >( c->data() )->innerSetPath( strPath );
}

void GLcell::innerSetPath( const string& strPath )
{
	strPath_ = strPath;
}

string GLcell::getPath( Eref e )
{
	return static_cast< const GLcell* >( e.data() )->strPath_;
}

void GLcell::setClientHost( const Conn* c, string strClientHost )
{
	static_cast< GLcell * >( c->data() )->innerSetClientHost( strClientHost );
}

void GLcell::innerSetClientHost( const string& strClientHost )
{
	strClientHost_ = strClientHost;
}

string GLcell::getClientHost( Eref e )
{
	return static_cast< const GLcell* >( e.data() )->strClientHost_;
}

void GLcell::setClientPort( const Conn* c, string strClientPort )
{
	static_cast< GLcell * >( c->data() )->innerSetClientPort( strClientPort );
}

void GLcell::innerSetClientPort( const string& strClientPort )
{
	strClientPort_ = strClientPort;
}

string GLcell::getClientPort( Eref e )
{
	return static_cast< const GLcell* >( e.data() )->strClientPort_;
}

void GLcell::setAttributeName( const Conn* c, string strAttributeName )
{
	static_cast< GLcell * >( c->data() )->innerSetAttributeName( strAttributeName );
}

void GLcell::innerSetAttributeName( const string& strAttributeName )
{
	strAttributeName_ = strAttributeName;
}

string GLcell::getAttributeName( Eref e )
{
	return static_cast< const GLcell* >( e.data() )->strAttributeName_;
}

void GLcell::setChangeThreshold( const Conn* c, double changeThreshold )
{
	static_cast< GLcell * >( c->data() )->innerSetChangeThreshold( changeThreshold );
}

void GLcell::innerSetChangeThreshold( const double changeThreshold )
{
	changeThreshold_ = changeThreshold;
}

double GLcell::getChangeThreshold( Eref e )
{
	return static_cast< const GLcell* >( e.data() )->changeThreshold_;
}

void GLcell::setSyncMode( const Conn* c, string syncMode )
{
	if ( syncMode == string( "on" ) )
		static_cast< GLcell * >( c->data() )->innerSetSyncMode( true );
	else if ( syncMode == string( "off" ) )
		static_cast< GLcell * >( c->data() )->innerSetSyncMode( false );
	else
		std::cerr << "Cannot set sync mode; argument must be either 'on' or 'off'." << std::endl;
}

void GLcell::innerSetSyncMode( const bool syncMode )
{
	syncMode_ = syncMode;
}

string GLcell::getSyncMode( Eref e )
{
	bool currentSyncMode = static_cast< const GLcell* >( e.data() )->syncMode_;

	if ( currentSyncMode )
		return string( "on" );
	else
		return string( "off" );
}

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void GLcell::reinitFunc( const Conn* c, ProcInfo info )
{
	static_cast< GLcell * >( c->data() )->reinitFuncLocal( c );
}

void GLcell::reinitFuncLocal( const Conn* c )
{
	double diameter, length, x0, y0, z0, x, y, z;

	/// Reload model geometry.
	// strPath_ should have been set.
	if ( !strPath_.empty() )
	{
		// renderList_ holds the flattened tree of elements to render.
		renderList_.clear();

		geometryData_.pathName = strPath_;
		geometryData_.renderListGLcellCompartments.clear();

		// Start populating renderList_ with the node in strPath_ 
		// and its children, recursively.
		add2RenderList( Shell::path2eid( strPath_, "/", 1 ) );

		for ( unsigned int i = 0; i < renderList_.size(); ++i )
		{

			if ( renderList_[i]()->cinfo()->isA( Cinfo::find( "Compartment" ) )
				&& get< double >( renderList_[i].eref(), "diameter", diameter )
				&& get< double >( renderList_[i].eref(), "length", length )
				&& get< double >( renderList_[i].eref(), "x0", x0 )
				&& get< double >( renderList_[i].eref(), "y0", y0 )
				&& get< double >( renderList_[i].eref(), "z0", z0 )
				&& get< double >( renderList_[i].eref(), "x", x )
				&& get< double >( renderList_[i].eref(), "y", y )
				&& get< double >( renderList_[i].eref(), "z", z ) )
			{
				GLcellCompartment glcellcomp;
				glcellcomp.diameter = diameter;
				glcellcomp.length = length;
				glcellcomp.x0 = x0;
				glcellcomp.y0 = y0;
				glcellcomp.z0 = z0;
				glcellcomp.x = x;
				glcellcomp.y = y;
				glcellcomp.z = z;
				
				geometryData_.renderListGLcellCompartments.push_back( glcellcomp );
			}
		}

		if ( strClientPort_.empty() )
			std::cerr << "GLcell error: Client port not specified." << std::endl;
		else if ( strClientHost_.empty() )
			std::cerr << "GLcell error: Client hostname not specified." << std::endl;
		else
			transmit( geometryData_, RESET );
	}
}

void GLcell::processFunc( const Conn* c, ProcInfo info )
{
	static_cast< GLcell * >( c->data() )->processFuncLocal( c->target(), info );
}

void GLcell::processFuncLocal( Eref e, ProcInfo info )
{
	double attr;
       
	if ( !renderList_.empty() )
	{
		renderListAttrsOld_ = renderListAttrs_;
		renderListAttrs_.clear();
		renderMapAttrsTransmitted_.clear();
		
		for ( unsigned int i = 0; i < renderList_.size(); ++i )
		{
			if ( renderList_[i]()->cinfo()->isA( Cinfo::find( "Compartment" ) ) 
			     && get< double >( renderList_[i].eref(), strAttributeName_.c_str(), attr ) )
			{
				renderListAttrs_.push_back( attr );
			}
		}

		for ( unsigned int j = 0; j < renderListAttrs_.size(); ++j )
		{
			if ( ( renderListAttrsOld_.size() == 0 )  || // on the first PROCESS after a RESET
			     syncMode_ ||                            // or we're in sync mode
			     ( fabs(renderListAttrs_[j] - renderListAttrsOld_[j]) > changeThreshold_ ) )
				renderMapAttrsTransmitted_[j] = renderListAttrs_[j];
		}

		if ( syncMode_ )
		{
			transmit( renderMapAttrsTransmitted_, PROCESSSYNC );
			receiveAckSyncMode(); // blocking call
		}
		else
			transmit( renderMapAttrsTransmitted_, PROCESS );
	}
}

///////////////////////////////////////////////////
// private function definitions
///////////////////////////////////////////////////

void GLcell::add2RenderList( Id id )
{
	vector< Id > children;
	Id found;

	// Save this node on renderList_, the flattened tree of elements to render.
	renderList_.push_back( id );
	
	// Determine this node's (immediate) children by tracing outgoing "childSrc" messages.
	Conn* i = id()->targets( "childSrc", 0 );
	for ( ; i->good(); i->increment() )
	{
		found = i->target()->id();

		children.push_back( found );
	}

	// If there are any children, call add2RenderList on each of them.
	for ( unsigned int j = 0; j < children.size(); ++j )
	{
		add2RenderList( children[j] );
	}
}

///////////////////////////////////////////////////
// networking helper function definitions
///////////////////////////////////////////////////

void* GLcell::getInAddress( struct sockaddr *sa )
{
	if ( sa->sa_family == AF_INET ) {
		return &( ( ( struct sockaddr_in* )sa )->sin_addr );
	}

	return &( ( ( struct sockaddr_in6* )sa )->sin6_addr );
}

int GLcell::getSocket( const char* hostname, const char* service )
{
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	
	memset( &hints, 0, sizeof hints );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( ( rv = getaddrinfo( hostname, service, &hints, &servinfo ) ) != 0 ) {
		std::cerr << "GLcell error: getaddrinfo: " << gai_strerror( rv ) << std::endl;
		return -1;
	}
	// loop through all the results and connect to the first we can
	for ( p = servinfo; p != NULL; p = p->ai_next ) {
		if ( ( sockFd_ = socket( p->ai_family, p->ai_socktype,
				     p->ai_protocol ) ) == -1 ) {
			std::cerr << "GLcell error: socket" << std::endl;
			continue;
		}
		
		if ( connect( sockFd_, p->ai_addr, p->ai_addrlen ) == -1 ) {
			close( sockFd_ );
			std::cerr << "GLcell error: connect" << std::endl;
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

int GLcell::sendAll( char* buf, int* len )
{
	int total = 0;        // how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n;

	while ( total < *len )
	{
		n = send( sockFd_, buf+total, bytesleft, 0 );
		if ( n == -1 )
		{
			std::cerr << "send error; errno: " << errno << " " << strerror( errno ) << std::endl;    
			break;
		}
		total += n;
		bytesleft -= n;
	}

	*len = total; // return number actually sent here

	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

int GLcell::receiveAckSyncMode()
{
	if ( ! isConnectionUp_ )
	{
		std::cerr << "Could not receive ACK in sync mode because the connection is down." << std::endl;
		return -1;
	}

	char ackBuf[1];
	int n;

	if ( ( n = recv( sockFd_, ackBuf, 1, 0) ) == 1 )
	{
		if ( *ackBuf == SYNCMODE_ACKCHAR )
		{
			return 1;
		}
	}
       
	return -2;
}

template< class T >
void GLcell::transmit( T& data, MSGTYPE messageType)
{
	if ( strClientHost_.empty() || strClientPort_.empty() ) // these should have been set.
		return;

	if ( ! isConnectionUp_ )
	{
		sockFd_ = getSocket( strClientHost_.c_str(), strClientPort_.c_str() );
		if ( sockFd_ == -1 ) 
		{
			std::cerr << "Couldn't connect to client!" << std::endl;
			return;
		}
	}

	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);

	archive << data;

	std::ostringstream headerStream;
	headerStream << std::setw( MSGSIZE_HEADERLENGTH )
		     << std::hex << archiveStream.str().size();

	headerStream << std::setw( MSGTYPE_HEADERLENGTH )
		     << messageType;

	int headerLen = headerStream.str().size() + 1;
	char* headerData = (char *) malloc( headerLen * sizeof( char ) );
	strcpy( headerData, headerStream.str().c_str() );
	
	if ( sendAll( headerData, &headerLen ) == -1 )
	{
		std::cerr << "Couldn't transmit header to client!" << std::endl;

		isConnectionUp_ = false;
		close( sockFd_ );
	}
	else
	{
		int archiveLen = archiveStream.str().size() + 1;
		char* archiveData = (char *) malloc( archiveLen * sizeof( char ) );
		strcpy( archiveData, archiveStream.str().c_str() );
				
		if ( sendAll( archiveData, &archiveLen ) == -1 )
		{
			std::cerr << "Couldn't transmit data to client!" << std::endl;	
		}
		free( archiveData );
	}
	free( headerData );
}

void GLcell::disconnect()
{
	if ( ! isConnectionUp_ )
	{
		sockFd_ = getSocket( strClientHost_.c_str(), strClientPort_.c_str() );
		if ( sockFd_ == -1 ) 
		{
			std::cerr << "Couldn't connect to client!" << std::endl;
			return;
		}
	}

	std::ostringstream headerStream;
	headerStream << std::setw( MSGSIZE_HEADERLENGTH ) << 0;
	headerStream << std::setw( MSGTYPE_HEADERLENGTH ) << DISCONNECT;

	int headerLen = headerStream.str().size() + 1;
	char* headerData = (char *) malloc( headerLen * sizeof( char ) );
	strcpy( headerData, headerStream.str().c_str() );


	if ( sendAll( headerData, &headerLen ) == -1 )
	{
		std::cerr << "Couldn't transmit DISCONNECT message to client!" << std::endl;
	}

	free( headerData );
	close( sockFd_ );
}
