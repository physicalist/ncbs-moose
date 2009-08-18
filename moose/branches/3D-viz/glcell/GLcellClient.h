/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <osg/ref_ptr>

#include "GLcellCompartment.h"

void receiveData();
void* getInAddr( struct sockaddr* sa );
int recvAll( int s, char* buf, int* len);
void updateGeometry( const std::vector< GLcellCompartment >& );

// the parent of the entire scene
osg::ref_ptr< osg::Geode > root_;
bool isGeometryDirty_;
char * port_;

const double EPSILON = 1e-8; // epsilon for floating-point comparison
const int HEADERLENGTH = 8;
const int BACKLOG = 10; // how many pending connections will be queued

// The data received from the MOOSE element GLcell
std::vector< GLcellCompartment > renderListGLcellCompartments_;		
