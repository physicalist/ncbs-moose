/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <osg/ref_ptr>
#include <osg/ShapeDrawable>
#include <osgViewer/Viewer>

#include "GLcellCompartment.h"

class GLcellClient
{
 public:
	GLcellClient( const char * port );
	void process();
	
	static const double EPSILON; // epsilon for floating-point comparison
	static const int HEADERLENGTH;
	static const int BACKLOG; // how many pending connections will be queued

 private:
	void* getInAddr( struct sockaddr* sa );
	int recvAll( int s, char* buf, int* len);

	const char * port_;

	/// The data received from the MOOSE element GLcell.
	std::vector< GLcellCompartment > renderListGLcellCompartments_;
	
	std::vector< osg::ref_ptr< osg::ShapeDrawable > > renderListShapes_;
	void renderRenderList();

	osg::ref_ptr< osgViewer::Viewer > viewer_;

};
