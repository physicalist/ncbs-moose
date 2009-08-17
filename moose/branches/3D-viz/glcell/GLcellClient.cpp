/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
//#include <boost/lexical_cast.hpp> // karan

#include <osg/ref_ptr>
#include <osg/Notify>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osg/Quat>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>

#include "GLcellCompartment.h"
#include "GLcellClient.h"

const double GLcellClient::EPSILON = 1e-9;
const int GLcellClient::HEADERLENGTH = 8;
const int GLcellClient::BACKLOG = 10;

/// Constructor opens the acceptor and starts waiting for the first incoming connection.
GLcellClient::GLcellClient( const char * port )
	:
	port_(port)
{
}

// get sockaddr, IPv4 or IPv6:
void* GLcellClient::getInAddr( struct sockaddr* sa )
{
	if ( sa->sa_family == AF_INET ) {
		return &( ( ( struct sockaddr_in* )sa )->sin_addr );
	}

	return &( ( ( struct sockaddr_in6* )sa )->sin6_addr );
}

int GLcellClient::recvAll( int s, char *buf, int *len )
{
	int total = 0;        // how many bytes we've received
	int bytesleft = *len; // how many we have left to receive
	int n;
	
	while ( total < *len )
	{
		n = recv( s, buf+total, bytesleft, 0 );
		if ( n == -1 )
		{
			std::cerr << "recv error; errno: " << errno << " " << strerror( errno ) << std::endl;
			break;
		}
		total += n;
		bytesleft -= n;
	}
	
	*len = total; /// return number actually received here
	
	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

void GLcellClient::process()
{
	int sockFd, newFd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage theirAddr; // connector's address information
	socklen_t sinSize;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	int numBytes, inboundDataSize;
	char header[HEADERLENGTH+1];
	char *buf;
	
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	
	if ( ( rv = getaddrinfo( NULL, port_, &hints, &servinfo ) ) != 0 ) {
		std::cerr << "getaddrinfo: " << gai_strerror( rv ) << std::endl;
		//return 1;
	}
	
	// loop through all the results and bind to the first we can
	for( p = servinfo; p != NULL; p = p->ai_next ) {
		if ( ( sockFd = socket( p->ai_family, p->ai_socktype, p->ai_protocol ) ) == -1 ) {
			std::cerr <<  "GLcellClient error: socket" << std::endl;
			continue;
		}
		
		if ( setsockopt( sockFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 ) {
			std::cerr << "GLcellClient error: setsockopt" << std::endl;
			exit( 1 );
		}
		
		if ( bind( sockFd, p->ai_addr, p->ai_addrlen ) == -1 ) {
			close( sockFd );
			std::cerr << "GLcellClient error: bind" << std::endl;
			continue;
		}
		
		break;
	}
	
	if ( p == NULL )  {
		std::cerr << "GLcellClient error: failed to bind" << std::endl;
		exit( 2 );
	}
  
	freeaddrinfo( servinfo ); // all done with this structure
  
	if ( listen( sockFd, BACKLOG ) == -1 ) {
		std::cerr << "GLcellClient error: listen" << std::endl;
		exit( 1 );
	}

	std::cout << "server: waiting for connections..." << std::endl;

	while ( 1 )            // main accept() loop
	{
		sinSize = sizeof( theirAddr );
		newFd = accept( sockFd, ( struct sockaddr * ) &theirAddr, &sinSize );
		if ( newFd == -1 ) {
			std::cerr << "GLcellClient error: accept" << std::endl;
			continue;
		}
		
		inet_ntop( theirAddr.ss_family, getInAddr( ( struct sockaddr * ) &theirAddr ), s, sizeof( s ) );
		std::cout << "GLcellClient error: connected to " << s << std::endl;
		
		numBytes = HEADERLENGTH+1;
		if ( recvAll( newFd, header, &numBytes ) == -1 ) {
			std::cerr << "GLcellClient error: recv" << std::endl;
			exit( 1 );
		}
				
		if ( numBytes < HEADERLENGTH+1 ) {
			std::cerr << "GLcellClient error: incomplete header received!" << std::endl;
			exit( 1 );
		}
		else {
			std::istringstream headerstream( std::string( header, HEADERLENGTH ) );
			headerstream >> std::hex >> inboundDataSize;
		}
		
		numBytes = inboundDataSize + 1;
		buf = ( char * ) malloc( ( numBytes ) * sizeof( char ) );
		
		if ( recvAll( newFd, buf, &numBytes ) == -1 ) {
			std::cerr << "GLcellClient error: recv" << std::endl;
			exit( 1 );
		}

		if ( numBytes < inboundDataSize+1 ) {
			std::cerr << "GLcellClient error: incomplete data received!" << std::endl;
			std::cerr << "numBytes: " << numBytes << " inboundDataSize: " << inboundDataSize << std::endl;
			exit( 1 );
		}
		else {
			std::istringstream archive_stream_i( std::string( buf, inboundDataSize ) );
			boost::archive::text_iarchive archive_i( archive_stream_i );
			
			archive_i >> renderListGLcellCompartments_;
		
			std::cout << renderListGLcellCompartments_.size() << std::endl;
		}
		
		free( buf );
		close( newFd );  // parent doesn't need this
	}
}

int main( int argc, char* argv[] )
{
	/// Check command line arguments.
	if ( argc != 2 )
	{
		std::cerr << "Usage: glcellclient <port>" << std::endl;
		return 1;
	}
	
	//unsigned short port = boost::lexical_cast< unsigned short >( argv[ 1 ] );
		
	GLcellClient::GLcellClient glcellclient( argv[ 1 ] );
	glcellclient.process();

	return 0;
}


    

