/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

////////////////////////////////////////////////////////////////////////////////
// The socket code is mostly taken from Beej's Guide to Network Programming   //
// at http://beej.us/guide/bgnet/. The original code is in the public domain. //
////////////////////////////////////////////////////////////////////////////////

#include <ctype.h>
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
#include <math.h>
#include <time.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/filesystem.hpp>

#include <osg/Vec3d>
#include <osg/ref_ptr>
#include <osg/Notify>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osg/Quat>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/Transform>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>

#include "CompartmentData.h"
#include "AckPickData.h"
#include "GeometryData.h"
#include "TextBox.h"
#include "GLCompartmentCylinder.h"
#include "GLCompartmentSphere.h"
#include "GLCompartment.h"

#include "GLcellClient.h"


bool KeystrokeHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor* )
{
	osgViewer::Viewer *viewer = dynamic_cast< osgViewer::Viewer* >( &aa );
	if ( ! viewer )
		return false;

	switch( ea.getEventType() )
	{
	case ( osgGA::GUIEventAdapter::PUSH ) :
	case ( osgGA::GUIEventAdapter::MOVE ) :
		x_ = ea.getX(); // record mouse location
		y_ = ea.getY();
		return false;
	case ( osgGA::GUIEventAdapter::RELEASE ) :
		// if the mouse hasn't moved since the last PUSH or MOVE,
		// perform a pick. otherwise let TrackBallManipulator handle this.
		if ( x_ == ea.getX() && y_ == ea.getY() )
		{
			if ( pick( ea.getXnormalized(), ea.getYnormalized(), viewer ) )
				return true;
		}
		return false;
	case ( osgGA::GUIEventAdapter::KEYDOWN ) :
		if ( ea.getKey() == 'c' || ea.getKey() == 'C' )
		{
			screenCaptureHandler_->captureNextFrame( *viewer_ );

			std::cout << "Saving screenshot. " << std::endl;
			return true;
		}
		else if ( ea.getKey() == 'm' || ea.getKey() == 'M' )
		{
			if ( isSavingMovie_ == true )
			{
				isSavingMovie_ = false;
				std::cout << "Stopping movie recording. " << std::endl;
			}
			else
			{			
				isSavingMovie_ = true;
				std::cout << "Starting movie recording... " << std::endl;
			}			
			return true;
		}
		else
			return false;
	default:
		break;
	}
	return false;
}

bool KeystrokeHandler::pick( const double x, const double y, osgViewer::Viewer* viewer )
{
	if ( ! viewer->getSceneData() )
		return false;

	double w = .05;
	double h = .05;

	osgUtil::PolytopeIntersector* picker = new osgUtil::PolytopeIntersector( osgUtil::Intersector::PROJECTION, 
										 x-w, y-h, x+w, y+h );
	osgUtil::IntersectionVisitor iv( picker );
	viewer->getCamera()->accept( iv );

	if ( picker->containsIntersections() )
	{
		const osg::NodePath& nodePath = picker->getFirstIntersection().nodePath;
		
		osg::Geode* geode = dynamic_cast< osg::Geode* >( nodePath[nodePath.size()-1] );
	        std::cout << mapGeode2Id_[geode] << " was picked." << std::endl;

		{
			boost::mutex::scoped_lock lock( mutexPickingDataUpdated_ );
			pickedId_ = mapGeode2Id_[geode];
		}
		isPickingDataUpdated_ = true;
	}
}

// get sockaddr, IPv4 or IPv6:
void* getInAddr( struct sockaddr* sa )
{
	if ( sa->sa_family == AF_INET ) {
		return &( ( ( struct sockaddr_in* )sa )->sin_addr );
	}

	return &( ( ( struct sockaddr_in6* )sa )->sin6_addr );
}

int sendAll( int socket, char* buf, int* len )
{
	int total = 0;        // how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n;

	while ( total < *len )
	{
		n = send( socket, buf+total, bytesleft, 0 );
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

int recvAll( int socket, char *buf, int *len )
{
	int total = 0;        // how many bytes we've received
	int bytesleft = *len; // how many we have left to receive
	int n;
	
	while ( total < *len )
	{
		n = recv( socket, buf+total, bytesleft, 0 );
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

void networkLoop( void )
{
	int newFd;

	while ( true )
	{
		if ( (newFd = acceptNewConnection( port_ )) != -1 )
		{
			receiveData( newFd );
		}		
		else
		{
			std::cerr << "Error in network loop... exiting." << std::endl;
			break;
		}
	}
}

int acceptNewConnection( char * port )
{
	int sockFd, newFd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage theirAddr; // connector's address information
	socklen_t sinSize;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	
	if ( ( rv = getaddrinfo( NULL, port, &hints, &servinfo ) ) != 0 ) {
		std::cerr << "getaddrinfo: " << gai_strerror( rv ) << std::endl;
		return -1;
	}
	
	// loop through all the results and bind to the first we can
	for( p = servinfo; p != NULL; p = p->ai_next ) {
		if ( ( sockFd = socket( p->ai_family, p->ai_socktype, p->ai_protocol ) ) == -1 ) {
			std::cerr <<  "GLcellClient error: socket" << std::endl;
			continue;
		}
		
		if ( setsockopt( sockFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) ) == -1 ) {
			std::cerr << "GLcellClient error: setsockopt" << std::endl;
			return -1;
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
		return -1;
	}
  
	freeaddrinfo( servinfo ); // all done with this structure
  
	if ( listen( sockFd, BACKLOG ) == -1 ) {
		std::cerr << "GLcellClient error: listen" << std::endl;
		return -1;
	}

	std::cout << "GLcellClient: waiting for connections..." << std::endl;

	sinSize = sizeof( theirAddr );
	newFd = accept( sockFd, ( struct sockaddr * ) &theirAddr, &sinSize );
	if ( newFd == -1 ) {
		std::cerr << "GLcellClient error: accept" << std::endl;
		return -1;
	}
		
	inet_ntop( theirAddr.ss_family, getInAddr( ( struct sockaddr * ) &theirAddr ), s, sizeof( s ) );

	std::cout << "GLcellClient: connected to " << s << std::endl;

	close( sockFd );
	return newFd;
}

void receiveData( int newFd )
{
	int numBytes, inboundDataSize;
	char header[MSGSIZE_HEADERLENGTH + MSGTYPE_HEADERLENGTH + 1];
	int messageType;
	char *buf;

	while ( true )
	{
		numBytes = MSGSIZE_HEADERLENGTH + MSGTYPE_HEADERLENGTH + 1;
		if ( recvAll( newFd, header, &numBytes ) == -1
             || numBytes < MSGSIZE_HEADERLENGTH + MSGTYPE_HEADERLENGTH + 1 ) 
		{
			std::cerr << "GLcellClient error:  could not receive message header!" << std::endl;
			break;
		}
		else {
			std::istringstream msgsizeHeaderstream( std::string( header, 
									     MSGSIZE_HEADERLENGTH ) );
			msgsizeHeaderstream >> std::hex >> inboundDataSize;
			
			std::istringstream msgtypeHeaderstream( std::string( header,
									     MSGSIZE_HEADERLENGTH,
									     MSGTYPE_HEADERLENGTH ) );
			msgtypeHeaderstream >> messageType;

			if ( messageType == DISCONNECT )
			{
				std::cout << "GLcellClient: MOOSE element disconnected." << std::endl;
				break;
			}
		}
		
		numBytes = inboundDataSize + 1;
		buf = ( char * ) malloc( numBytes * sizeof( char ) );
		
		if ( recvAll( newFd, buf, &numBytes ) == -1 
            || numBytes < inboundDataSize + 1 )
		{
			std::cerr << "GLcellClient error: incomplete data received!" << std::endl;
			std::cerr << "numBytes: " << numBytes << " inboundDataSize: " << inboundDataSize << std::endl;
			break;
		}
		else {
			std::istringstream archive_stream_i( std::string( buf, inboundDataSize ) );
			boost::archive::text_iarchive archive_i( archive_stream_i );
			
			if ( messageType == RESET) 
			{
				geometryData_.renderListCompartmentData.clear();
				archive_i >> geometryData_;

				updateGeometry( geometryData_ );
			}
			else if ( messageType == PROCESS || messageType == PROCESSSYNC )
			{

				{  // additional scope to wrap scoped_lock
					boost::mutex::scoped_lock lock( mutexColorSetSaved_ );
					
					renderMapAttrs_.clear();
					archive_i >> renderMapAttrs_;
				}
				


				// wait for display to update if in sync mode
				if ( messageType == PROCESS )
				{
					if ( isColorSetDirty_ == true ) // We meant to set colorset dirty but
						// it is already dirty which means the rendering thread has not picked it up yet.
					{
						std::cerr << "skipping frame..." << std::endl;
					}
	
					isColorSetDirty_ = true;

				}
				else // messageType == PROCESSSYNC
				{
					isColorSetDirty_ = true;
				
					// wait for the updated color set to render to display
					{
						boost::mutex::scoped_lock lock( mutexColorSetUpdated_ );
						while ( isColorSetDirty_ )
						{
							condColorSetUpdated_.wait( lock );
						}
					}
				}
				
				sendAck( newFd );
			}
		}

		free( buf );
	}

	close( newFd );	
}

void sendAck( int socket )
{
	// send back AckPickData structure
	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive( archiveStream );
				

	if ( isPickingDataUpdated_ )
	{
		AckPickData newPick;
		newPick.wasSomethingPicked = true;
		{
			boost::mutex::scoped_lock lock( mutexPickingDataUpdated_ );
			newPick.idPicked = pickedId_;
		}
		archive << newPick;
		isPickingDataUpdated_ = false;
	}
	else
	{
		AckPickData noPicks;
		noPicks.wasSomethingPicked = false;
		noPicks.idPicked = 0;

		archive << noPicks;
	}

	std::ostringstream headerStream;
	headerStream << std::setw( MSGSIZE_HEADERLENGTH )
		     << std::hex << archiveStream.str().size();

	int headerLen = headerStream.str().size() + 1;
	char *headerData = ( char * ) malloc( headerLen * sizeof( char ) );
	strcpy( headerData, headerStream.str().c_str() );

	if ( sendAll( socket, headerData, &headerLen ) == -1
         || headerLen < headerStream.str().size() + 1 )
	{
		std::cerr << "glcellclient error: couldn't transmit Ack header to GLcell!" << std::endl;
		close( socket );
	}
	else
	{		
		int archiveLen = archiveStream.str().size() + 1;
		char* archiveData = ( char * ) malloc( archiveLen * sizeof( char ) );
		strcpy( archiveData, archiveStream.str().c_str() );

		if ( sendAll( socket, archiveData, &archiveLen ) == -1
             || archiveLen < archiveStream.str().size() + 1 )
		{
			std::cerr << "glcellclient error: couldn't transmit Ack to GLcell!" << std::endl;
		}

		free( archiveData );
	}
	free( headerData );
}

void updateGeometry( GeometryData geometryData )
{	
	const std::vector< CompartmentData >& compartments = geometryData.renderListCompartmentData;
	
	if ( mapId2GLCompartment_.size() > 0 )
	{
		std::map< unsigned int, GLCompartment *>::iterator id2glcompIterator;

		for ( id2glcompIterator = mapId2GLCompartment_.begin();
		      id2glcompIterator != mapId2GLCompartment_.end();
		      id2glcompIterator++ )
		{
			delete id2glcompIterator->second;
		}

		mapId2GLCompartment_.clear();
		mapGeode2Id_.clear();
	}

	root_ = new osg::Group; // root_ is an osg::ref_ptr
	root_->setDataVariance( osg::Object::STATIC );

	if ( textParent_ != NULL)
		delete textParent_;
	textParent_ = new TextBox();
	textParent_->setPosition( osg::Vec3d( 10, 10, 0 ) );
	textParent_->setText( geometryData.pathName );
	root_->addChild( textParent_->getGroup() );

	// First pass: create the basic hollow cylinders with no end-caps
	for ( int i = 0; i < compartments.size(); ++i )
	{
		const unsigned int& id = compartments[i].id;
		const double& diameter = compartments[i].diameter;
		const double& length = compartments[i].length;
		const double& x0 = compartments[i].x0;
		const double& y0 = compartments[i].y0;
		const double& z0 = compartments[i].z0;
		const double& x = compartments[i].x;
		const double& y = compartments[i].y;
		const double& z = compartments[i].z;
			
		if ( length < SIZE_EPSILON ) 
			// i.e., length is zero so the compartment is spherical
		{ 
			GLCompartmentSphere* sphere = new GLCompartmentSphere( osg::Vec3f( (x0+x)/2, (y0+y)/2, (z0+z)/2 ),
									       diameter/2,
									       incrementAngle_ );
			mapId2GLCompartment_[id] = dynamic_cast< GLCompartment* >( sphere ); // to call the polymorphic function setColor() later

			osg::Geometry* sphereGeom = sphere->getGeometry();
			
			osg::Geode* geode = new osg::Geode;
			geode->addDrawable( sphereGeom );
			root_->addChild( geode );
			
			mapGeode2Id_[geode] = id;
		}
		else 
			// the compartment is cylindrical
		{ 
			// GLCompartmentCylinders (like OSG::Cylinders) are oriented
			// by default along (0,0,1). To orient them
			// according to readcell's specification of (the line joining) the
			// centers of their circular faces, they must be rotated around 
			// an axis given by the cross-product of the initial and final
			// vectors, by an angle given by the dot-product of the same.
					
			// Also note that although readcell's conventions for 
			// the x,y,z directions differ from OSG's, (in OSG z is down to up
			// and y is perpendicular to the display, pointing inward),
			// OSG's viewer chooses a suitable viewpoint by default.

			osg::Vec3f initial( 0.0f, 0.0f, 1.0f );
			initial.normalize();

			osg::Vec3f final( x-x0, y-y0, z-z0 );
			final.normalize();

			osg::Quat::value_type angle = acos( initial * final );
			osg::Vec3f axis = initial ^ final;
			axis.normalize();

			GLCompartmentCylinder* cylinder = new GLCompartmentCylinder( osg::Vec3f( (x0+x)/2, (y0+y)/2, (z0+z)/2 ),
										     osg::Quat( angle, axis ),
										     length,
										     diameter / 2,
										     incrementAngle_ );
			
			mapId2GLCompartment_[id] = dynamic_cast< GLCompartment* >( cylinder ); // to call the polymorphic function setColor() later

			osg::Geometry* cylinderGeom = cylinder->getGeometry();

			osg::Geode* geode = new osg::Geode;
			geode->addDrawable( cylinderGeom );
			root_->addChild( geode );
			
			mapGeode2Id_[geode] = id;
		}
	}

	// Second pass: for cylinders only, find neighbours and create interpolated joints
	for ( int i = 0; i < compartments.size(); ++i )
	{
		const unsigned int& id = compartments[i].id;
		const std::vector< unsigned int > vNeighbourIds = compartments[i].vNeighbourIds;

		for ( int j = 0; j < vNeighbourIds.size(); ++j )
		{
			if ( mapId2GLCompartment_[id]->getCompartmentType() == CYLINDER &&
			     mapId2GLCompartment_[vNeighbourIds[j]]->getCompartmentType() == CYLINDER )
			{
				dynamic_cast< GLCompartmentCylinder* >( mapId2GLCompartment_[id] )->
					addHalfJointToNeighbour( dynamic_cast< GLCompartmentCylinder* >( mapId2GLCompartment_[vNeighbourIds[j]] ) );
			}
		} 
	 }

	// Third pass: for cylinders only, form hemispherical end-caps on any joints not yet attached to neighbours.
	for ( int i = 0; i < compartments.size(); ++i )
	{
		const unsigned int& id = compartments[i].id;

		if ( mapId2GLCompartment_[id]->getCompartmentType() == CYLINDER)
		{
			dynamic_cast< GLCompartmentCylinder* >( mapId2GLCompartment_[id] )->closeOpenEnds();
		}
	}


	isGeometryDirty_ = true;

}

void draw()
{
	viewer_ = new osgViewer::Viewer;
	viewer_->setThreadingModel( osgViewer::Viewer::SingleThreaded );

	viewer_->setSceneData( new osg::Geode );
	
	osg::ref_ptr< osg::GraphicsContext::Traits > traits = new osg::GraphicsContext::Traits;
	traits->x = WINDOW_OFFSET_X; // window x offset in window manager
	traits->y = WINDOW_OFFSET_Y; // likewise, y offset ...
	traits->width = WINDOW_WIDTH;
	traits->height = WINDOW_HEIGHT;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;

	osg::ref_ptr< osg::GraphicsContext > gc = osg::GraphicsContext::createGraphicsContext( traits.get() );
	
	viewer_->getCamera()->setClearColor( osg::Vec4( 0., 0., 0., 1. ) ); // black background
	viewer_->getCamera()->setGraphicsContext( gc.get() );
	viewer_->getCamera()->setViewport( new osg::Viewport( 0, 0, traits->width, traits->height ) );
		
	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	viewer_->getCamera()->setDrawBuffer( buffer );
	viewer_->getCamera()->setReadBuffer( buffer );

	viewer_->realize();
	viewer_->setCameraManipulator( new osgGA::TrackballManipulator );
	viewer_->addEventHandler( new osgViewer::StatsHandler );
	viewer_->addEventHandler( new KeystrokeHandler );

	screenCaptureHandler_ = new osgViewer::ScreenCaptureHandler( new osgViewer::ScreenCaptureHandler::WriteToFile::WriteToFile( getSaveFilename(), "jpg", osgViewer::ScreenCaptureHandler::WriteToFile::SEQUENTIAL_NUMBER ) );
	viewer_->addEventHandler( screenCaptureHandler_ );

	std::map< unsigned int, double >::iterator renderMapAttrsIterator;

	while ( !viewer_->done() ) {

		if ( isGeometryDirty_ ) {
			isGeometryDirty_ = false;
			
			viewer_->setSceneData( root_ );
		}

		if ( isColorSetDirty_ ) {
			boost::mutex::scoped_lock lock( mutexColorSetSaved_ );
			
			for ( renderMapAttrsIterator = renderMapAttrs_.begin();
			      renderMapAttrsIterator != renderMapAttrs_.end();
			      renderMapAttrsIterator++ )
			{
				unsigned int id = renderMapAttrsIterator->first;
				double attr = renderMapAttrsIterator->second;

				GLCompartment* glcompartment = mapId2GLCompartment_[id];
				double red, green, blue;
				
				if ( attr > highValue_ )
				{
					red   = colormap_[ colormap_.size()-1 ][ 0 ];
					green = colormap_[ colormap_.size()-1 ][ 1 ];
					blue  =  colormap_[ colormap_.size()-1 ][ 2 ];
				}
				else if ( attr < lowValue_ )
				{
					red   = colormap_[ 0 ][ 0 ];
					green = colormap_[ 0 ][ 1 ];
					blue  = colormap_[ 0 ][ 2 ];
				}
				else
				{
					double intervalSize = ( highValue_ - lowValue_ ) / colormap_.size();
					int ix = static_cast< int >( floor( ( attr - lowValue_ ) / intervalSize ) );

					red   = colormap_[ ix ][ 0 ];
					green = colormap_[ ix ][ 1 ];
					blue  = colormap_[ ix ][ 2 ];
				}

				glcompartment->setColor( osg::Vec4( red, green, blue, 1.0f ) );
			}

			{
				boost::mutex::scoped_lock lock( mutexColorSetUpdated_ );
				isColorSetDirty_ = false;
				
				condColorSetUpdated_.notify_one(); // no-op except when responding to PROCESSSYNC
			}
		}

		if ( isSavingMovie_ )
			screenCaptureHandler_->captureNextFrame( *viewer_ );

		viewer_->frame();
	}
}

std::string getSaveFilename( void )
{
	std::stringstream filename;
	filename << "Screenshot_";

	/*char strTime[26];
	time_t t;
	time( &t );
	strcpy( strTime, ctime( &t ) );
	strTime[24] = '\0'; // strip '\n'

	filename << strTime;*/

	filename << static_cast<long>( time( NULL ) );
	
	boost::filesystem::path fullPath( saveDirectory_ / filename.str() );
	return fullPath.string();
}

int main( int argc, char* argv[] )
{
	int c;
	char* strHelp = "Usage: glcellclient\n"
		"\t-p <number>: port number\n"
		"\t-c <string>: filename of colormap file\n"
		"\t[-u <number>: value represented by colour on last line of colormap file (default is 0.05V)]\n"
		"\t[-l <number>: value represented by colour on first line of colormap file (default if -0.1V)]\n"
		"\t[-d <string>: pathname in which to save screenshots and sequential image files (default is ./)]\n"
		"\t[-a <number>: required to be between 1 and 60 degrees; this value represents angular increments in drawing the sides of curved bodies; smaller numbers give smoother bodies]\n";
	
	bool isValid;
	// Check command line arguments.
	while ( ( c = getopt( argc, argv, "hp:c:u:l:d:a:" ) ) != -1 )
		switch( c )
		{
		case 'h':
			std::cout << strHelp << std::endl;
			return 1;
		case 'p':
			port_ = optarg;
			break;
		case 'c':
			fileColormap_ = optarg;
			break;
		case 'u':
			highValue_ = strtod( optarg, NULL );
			break;
		case 'l':
			lowValue_ = strtod( optarg, NULL );
			break;
		case 'd':
			saveDirectory_ = optarg;
			isValid = boost::filesystem::is_directory(saveDirectory_);
			if ( !isValid )
			{
				printf( "Argument to option -d must be a valid directory name.\n" );
				return 1;
			}
			break;
		case 'a':
			double value = strtod( optarg, NULL );
			if ( value < 1 )
				incrementAngle_ = 1;
			else if ( value > 60 )
				incrementAngle_ = 60;
			else
				incrementAngle_ = value;
			break;
		case '?':
			if ( optopt == 'p' || optopt == 'c' || optopt == 'u' || optopt == 'l' || optopt == 'd' )
				printf( "Option -%c requires an argument.\n", optopt );
			else
				printf( "Unknown option -%c.\n", optopt );
			return 1;
		default:
			return 1;
		}
	
	if ( port_ == NULL || fileColormap_ == NULL )
	{
		std::cerr << "Port number and colormap filename are required arguments." << std::endl;
		std::cerr << strHelp << std::endl;
		return 1;
	}
	if ( highValue_ <= lowValue_ )
	{
		std::cerr << "-u argument must be greater than -l argument." << std::endl;
		std::cerr << strHelp << std::endl;
		return 1;
	}

	// parse colormap file
	std::ifstream fColormap;
	std::string lineToSplit;
	char* pEnd;

	fColormap.open( fileColormap_ );
	if ( ! fColormap.is_open() )
	{
		std::cerr << "Couldn't open colormap file: " << fileColormap_ << "!" << std::endl;
		return 2;
	}
	else
	{
		while ( ! fColormap.eof() )
		{
			getline( fColormap, lineToSplit );
			if ( lineToSplit.length() > 0 )  // not a blank line (typically the last line)
			{
				osg::Vec3d color;
				color[0] = strtod( lineToSplit.c_str(), &pEnd ) / 65535.;
				color[1] = strtod( pEnd, &pEnd ) / 65535.;
				color[2] = strtod( pEnd, NULL ) / 65535.;
				
				colormap_.push_back( color );
			}
		}
	}
	fColormap.close();
	
	// launch network thread and run the GUI in the main loop
	boost::thread threadProcess( networkLoop );
	draw();

	return 0;
}
