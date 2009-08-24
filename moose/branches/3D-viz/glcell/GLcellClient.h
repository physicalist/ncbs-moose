/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <osg/ref_ptr>
#include <osg/Vec3d>

#include "GLcellCompartment.h"
#include "boost/thread/mutex.hpp"

enum MSGTYPE
{
	RESET,
	PROCESS,
	DISCONNECT
};


void networkLoop();
int acceptNewConnection( char * port );
void receiveData( int newFd );

void* getInAddr( struct sockaddr* sa );
int recvAll( int s, char* buf, int* len);
void updateGeometry( const std::vector< GLcellCompartment >& );
void updateColorSet();

// the parent of the entire scene
osg::ref_ptr< osg::Geode > root_;
bool isGeometryDirty_ = false;
bool isColorSetDirty_ = false;

char * port_ = NULL;

char * fileColormap_ = NULL;
double highVoltage_ = 0.05;
double lowVoltage_ = -0.1;

const double SIZE_EPSILON = 1e-8; // epsilon for minimum compartent size
const double FP_EPSILON = 1e-8; // epsilon for floating-point comparison
const int MSGTYPE_HEADERLENGTH = 1;
const int MSGSIZE_HEADERLENGTH = 8;
const int BACKLOG = 10; // how many pending connections will be queued

// Data received from the MOOSE element GLcell:
//   Geometry, received in RESET step:
std::vector< GLcellCompartment > renderListGLcellCompartments_;	
//   Potential, received in PROCESS step:
std::vector< double > renderListVms_;

boost::mutex mutexColorSet_;
std::vector< osg::Vec3d > colormap_;

std::vector< osg::Vec3d > oldColorsPerDrawable_;
