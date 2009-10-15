/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "Constants.h"
#include "AckPickData.h"

class KeystrokeHandler : public osgGA::GUIEventHandler
{
 public:
 KeystrokeHandler() :
	isCurrentProjectionOrtho_(false)
	{}
	
	virtual bool handle( const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&, osg::Object*, osg::NodeVisitor* );

 private:
	~KeystrokeHandler() {}
	bool pick( const double x, const double y, osgViewer::Viewer* viewer );
	void switchProjection( osgViewer::Viewer* viewer );

	double x_;
	double y_;

	double isCurrentProjectionOrtho_;
};

enum MSGTYPE
{
	RESET,
	PROCESS,
	PROCESSSYNC,
	DISCONNECT
};

void networkLoop( void );
int acceptNewConnection( char * port );
void receiveData( int newFd );

void* getInAddr( struct sockaddr* sa );
int sendAll( int socket, char* buf, int* len );
int recvAll( int socket, char* buf, int* len);
void sendAck( int socket );
void updateGeometry( GeometryData geometry );

std::string getSaveFilename( void );

osg::ref_ptr< osg::Group > root_;
TextBox::TextBox* textParent_ = NULL;

std::map< unsigned int, GLCompartment* > mapId2GLCompartment_; // this is used to call the polymorphic function setColor()
std::map< osg::Geode*, unsigned int > mapGeode2Id_; // this is used to obtain the id of a compartment that the user has picked with the mouse

volatile bool isGeometryDirty_ = false;
volatile bool isColorSetDirty_ = false;


volatile bool isPickingDataUpdated_ = false;
boost::mutex mutexPickingDataUpdated_;
unsigned int pickedId_;

char * port_ = NULL;
char * fileColormap_ = NULL;
double highValue_ = 0.05;
double lowValue_ = -0.1;
double incrementAngle_ = DEFAULT_INCREMENT_ANGLE;

const int MSGTYPE_HEADERLENGTH = 1;
const int MSGSIZE_HEADERLENGTH = 8;
const int BACKLOG = 10; // how many pending connections will be queued

bool isSavingMovie_ = false;
boost::filesystem::path saveDirectory_(".");

// Data received from the MOOSE element GLcell:
//   Info and geometry, received in RESET step:
GeometryData geometryData_;

// Attribute values mapped to colors, received in PROCESS step:
std::map< unsigned int, double > renderMapAttrs_;

boost::mutex mutexColorSetSaved_;
boost::mutex mutexColorSetUpdated_;
boost::condition condColorSetUpdated_;

std::vector< osg::Vec3d > colormap_;

osgViewer::ScreenCaptureHandler* screenCaptureHandler_;
osgViewer::Viewer* viewer_;

