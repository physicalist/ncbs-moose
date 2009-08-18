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
#include <boost/serialization/vector.hpp>
#include <boost/thread/thread.hpp>

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

// The socket code is mostly taken from Beej's Guide to Network Programming
// at http://beej.us/guide/bgnet/. It is in the public domain.

// get sockaddr, IPv4 or IPv6:
void* getInAddr( struct sockaddr* sa )
{
	if ( sa->sa_family == AF_INET ) {
		return &( ( ( struct sockaddr_in* )sa )->sin_addr );
	}

	return &( ( ( struct sockaddr_in6* )sa )->sin6_addr );
}

int recvAll( int s, char *buf, int *len )
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

void receiveData()
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
		std::cout << "GLcellClient: receiving data from " << s << std::endl;
		
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
		
		std::cout << "bytes: " << inboundDataSize << std::endl; // karan
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
			
			renderListGLcellCompartments_.clear();
			archive_i >> renderListGLcellCompartments_;

			updateGeometry(renderListGLcellCompartments_);
		}
		
		free( buf );
		close( newFd );  // parent doesn't need this
	}
}

void updateGeometry( const std::vector< GLcellCompartment >& compartments )
{
	/*unsigned int numDrawables = root_->getNumDrawables();
	if ( numDrawables > 0 )
	root_->removeDrawables( 0, numDrawables );*/

	root_ = new osg::Geode;

	for (int i = 0; i < compartments.size(); ++i) {
		const double& diameter = compartments[i].diameter;
		const double& length = compartments[i].length;
		const double& x0 = compartments[i].x0;
		const double& y0 = compartments[i].y0;
		const double& z0 = compartments[i].z0;
		const double& x = compartments[i].x;
		const double& y = compartments[i].y;
		const double& z = compartments[i].z;
		const double& Vm = compartments[i].Vm;
			
		if ( length < EPSILON ) { // i.e., length is zero so the compartment is spherical
			osg::Sphere* sphere = new osg::Sphere( osg::Vec3f( (x0+x)/2, (y0+y)/2, (z0+z)/2 ),
							       diameter/2 );
			osg::ShapeDrawable* drawable = new osg::ShapeDrawable( sphere );
			root_->addDrawable( drawable ); // addDrawable increments ref count of drawable
		}
		else { // the compartment is cylindrical
			osg::Cylinder* cylinder = new osg::Cylinder( osg::Vec3f( (x0+x)/2, (y0+y)/2, (z0+z)/2 ), 
								     diameter/2, length );

			/// OSG Cylinders are initially oriented along (0,0,1). To orient them
			/// according to readcell's specification of (the line joining) the
			/// centers of their circular faces, they must be rotated around 
			/// an axis given by the cross-product of the initial and final
			/// vectors, by an angle given by the dot-product of the same.
					
			/// Also note that although readcell's conventions for the x,y,z directions
			/// differ from OSG's, (in OSG z is down to up and y is perpendicular 
			/// to the display, pointing inward), OSG's viewer chooses
			/// a suitable viewpoint by default.

			osg::Vec3f initial( 0.0f, 0.0f, 1.0f);
			initial.normalize();

			osg::Vec3f final( x-x0, y-y0, z-z0 );
			final.normalize();

			osg::Quat::value_type angle = acos( initial * final );
			osg::Vec3f axis = initial ^ final;
			axis.normalize();

			cylinder->setRotation( osg::Quat( angle, axis ) ) ;

			osg::ShapeDrawable* drawable = new osg::ShapeDrawable( cylinder );
			root_->addDrawable( drawable );
		}
	}

	isGeometryDirty_ = true;
	// Note: we don't bother with locks for sharing isGeometryDirty_ between updateGeometry()
	// and draw(), because of the vastly different timescales. RESETs are sent manually and it
	// is very unlikely that two RESETs will arrive consecutively quickly enough to interrupt
	// an update in draw(), and acquiring a lock to check isGeometryDirty_ on every frame will
	// be expensive.

}

void draw()
{
	osg::ref_ptr< osgViewer::Viewer > viewer = new osgViewer::Viewer;
	
	viewer->setSceneData( new osg::Geode ); // placeholder Geode to be reclaimed during first update
	
	osg::ref_ptr< osg::GraphicsContext::Traits > traits = new osg::GraphicsContext::Traits;
	traits->x = 50; // window x offset in window manager
	traits->y = 50; // likewise, y offset ...
	traits->width = 600;
	traits->height = 600;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	osg::ref_ptr< osg::GraphicsContext > gc = osg::GraphicsContext::createGraphicsContext( traits.get() );
	
	viewer->getCamera()->setClearColor( osg::Vec4( 0., 0., 0., 1. ) ); // black background
	viewer->getCamera()->setGraphicsContext( gc.get() );
	viewer->getCamera()->setViewport( new osg::Viewport( 0, 0, traits->width, traits->height ) );
		
	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	viewer->getCamera()->setDrawBuffer( buffer );
	viewer->getCamera()->setReadBuffer( buffer );

	viewer->realize();
	viewer->setCameraManipulator(new osgGA::TrackballManipulator());

	while ( !viewer->done() ) {
		if ( isGeometryDirty_ ) {
			isGeometryDirty_ = false;
			viewer->setSceneData( root_.get() );
		}
		viewer->frame();
	}
}

int main( int argc, char* argv[] )
{
	/// Check command line arguments.
	if ( argc != 2 ) {
		std::cerr << "Usage: glcellclient <port>" << std::endl;
		return 1;
	}

	isGeometryDirty_ = false;
	port_ = argv[ 1 ];
		
	boost::thread threadProcess( receiveData );
	draw();

	return 0;
}
