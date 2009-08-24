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
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

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
	strClientPort_( "" )
{
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

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

void GLcell::processFunc( const Conn* c, ProcInfo info )
{
	static_cast< GLcell * >( c->data() )->processFuncLocal( c->target(), info );
}

void GLcell::processFuncLocal( Eref e, ProcInfo info )
{
	double Vm;
       
	if ( !renderList_.empty() )
	{
		renderListVms_.clear();
		
		for ( int i = 0; i < renderList_.size(); ++i )
		{
			if ( renderList_[i]()->cinfo()->isA( Cinfo::find( "Compartment" ) ) 
			     && get< double >( renderList_[i].eref(), "Vm", Vm) )
			{
				renderListVms_.push_back( Vm );
			}
		}

		transmit( renderListVms_, PROCESS );
	}
}

void GLcell::reinitFunc( const Conn* c, ProcInfo info )
{
	static_cast< GLcell * >( c->data() )->reinitFuncLocal( c );
}

void GLcell::reinitFuncLocal( const Conn* c )
{
	double diameter, length, x0, y0, z0, x, y, z, Vm;

	/// Reload model geometry.
	// strPath_ should have been set.
	if ( !strPath_.empty() )
	{
		// renderList_ holds the flattened tree of elements to render.
		// renderListShapes_ holds a corresponding list of osg::ShapeDrawables.
		renderList_.clear();
		renderListGLcellCompartments_.clear();

		// Start populating renderList_ with the node in strPath_ 
		// and its children, recursively.
		add2RenderList( Shell::path2eid( strPath_, "/", 1 ) );
			
		for ( int i = 0; i < renderList_.size(); ++i )
		{

			if ( renderList_[i]()->cinfo()->isA( Cinfo::find( "Compartment" ) )
				&& get< double >( renderList_[i].eref(), "diameter", diameter )
				&& get< double >( renderList_[i].eref(), "length", length )
				&& get< double >( renderList_[i].eref(), "x0", x0 )
				&& get< double >( renderList_[i].eref(), "y0", y0 )
				&& get< double >( renderList_[i].eref(), "z0", z0 )
				&& get< double >( renderList_[i].eref(), "x", x )
				&& get< double >( renderList_[i].eref(), "y", y )
				&& get< double >( renderList_[i].eref(), "z", z )
				&& get< double >( renderList_[i].eref(), "Vm", Vm ) )
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
				glcellcomp.Vm = Vm;
				
				renderListGLcellCompartments_.push_back( glcellcomp );
			
			}
		}

		transmit( renderListGLcellCompartments_, RESET );
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
	for ( int j = 0; j < children.size(); ++j )
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
	int sockfd;
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
		if ( ( sockfd = socket( p->ai_family, p->ai_socktype,
				     p->ai_protocol ) ) == -1 ) {
			std::cerr << "GLcell error: socket" << std::endl;
			continue;
		}
		
		if ( connect( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) {
			close( sockfd );
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
	
	return sockfd;
}

int GLcell::sendAll( int s, char* buf, int* len )
{
	int total = 0;        // how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n;

	while( total < *len )
	{
		n = send( s, buf+total, bytesleft, 0 );
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

template< class T >
void GLcell::transmit( T& data, MSGTYPE messageType)
{
	if ( !strClientPort_.empty() ) // strClientPort_ should have been set.
	{
		int sockFd;
		sockFd = getSocket( strClientHost_.c_str(), strClientPort_.c_str() );

		if ( sockFd == -1 ) 
		{
			std::cerr << "Couldn't connect to client!" << std::endl;
		}
		else
		{
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
	
			if (sendAll( sockFd, headerData, &headerLen ) == -1 )
			{
				std::cerr << "Couldn't transmit header to client!" << std::endl;
			}
			else
			{
				int archiveLen = archiveStream.str().size() + 1;
				char* archiveData = (char *) malloc( archiveLen * sizeof( char ) );
				strcpy( archiveData, archiveStream.str().c_str() );
				
				if ( sendAll( sockFd, archiveData, &archiveLen ) == -1 )
				{
					std::cerr << "Couldn't transmit data to client!" << std::endl;	
				}
				free( archiveData );
			}
			free( headerData );
			
		}
		close( sockFd );
	}
	else
	{
		std::cerr << "GLcell error: Client port not specified." << std::endl;
	}

}
