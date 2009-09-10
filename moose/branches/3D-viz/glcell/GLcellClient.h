/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

class KeystrokeHandler : public osgGA::GUIEventHandler
{
 public:
	KeystrokeHandler() {}

	virtual bool handle( const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&, osg::Object*, osg::NodeVisitor* );
 private:
	~KeystrokeHandler() {}
};

/////////////////////////////////////////////////////////////////////////////////////
// The class TextBox is derived from the OSG tutorial and code at		   //
// http://www.cs.clemson.edu/~malloy/courses/3dgames-2007/tutor/web/text/text.html //
/////////////////////////////////////////////////////////////////////////////////////

class TextBox
{
 public:
	TextBox();
	~TextBox() {}
	
	void setText( const std::string& text );
	void setFont( const std::string& font );
	void setColor( osg::Vec4d color );
	void setPosition( osg::Vec3d position ); // (x,y,z) where z is always 0; (0,0,0) is bottom-left
	void setTextSize( unsigned int size );

	osg::Group& getGroup() const;
	std::string getText() const;
 private:
	osg::MatrixTransform* matrixTransform_; // protects HUD from transformations of parent node
	osg::Projection* projection_; // provides surface to render text
	osg::Geode* textGeode_; // hosts the text drawable
	osgText::Text* text_;
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
int recvAll( int s, char* buf, int* len);
void updateGeometry( GeometryData geometry );

std::string getSaveFilename( void );

osg::ref_ptr< osg::Group > root_;
osg::ref_ptr< osg::Geode > geomParent_;
TextBox::TextBox* textParent_ = NULL;

volatile bool isGeometryDirty_ = false;
volatile bool isColorSetDirty_ = false;

char * port_ = NULL;

char * fileColormap_ = NULL;
double highValue_ = 0.05;
double lowValue_ = -0.1;

const double SIZE_EPSILON = 1e-8; // floating-point (FP) epsilon for 
                                  // ... minimum compartment size
const double FP_EPSILON = 1e-8;   // FP epsilon for comparison

const int WINDOW_OFFSET_X = 50;
const int WINDOW_OFFSET_Y = 50;
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;

const int MSGTYPE_HEADERLENGTH = 1;
const int MSGSIZE_HEADERLENGTH = 8;
const int BACKLOG = 10; // how many pending connections will be queued
const char SYNCMODE_ACKCHAR = '*';

bool isSavingMovie_ = false;
boost::filesystem::path saveDirectory_(".");

// Data received from the MOOSE element GLcell:
//   Info and geometry, received in RESET step:
GeometryData geometryData_;

// Attribute values mapped to colors, received in PROCESS step:
std::map< int, double > renderMapAttrs_;

boost::mutex mutexColorSetSaved_;
boost::mutex mutexColorSetUpdated_;
boost::condition condColorSetUpdated_;

std::vector< osg::Vec3d > colormap_;

osgViewer::ScreenCaptureHandler* screenCaptureHandler_;
osgViewer::Viewer* viewer_;

