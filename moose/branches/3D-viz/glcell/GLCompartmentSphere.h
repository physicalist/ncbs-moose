/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "GLCompartment.h"

class GLCompartmentSphere : public GLCompartment
{
public:
	GLCompartmentSphere( osg::Vec3 position, double radius, double incrementAngle );
	~GLCompartmentSphere();

	osg::ref_ptr< osg::Geometry > getGeometry();
	void setColor( osg::Vec4 color );

	int getCompartmentType();

private:
	void addHemisphericalCap( bool leftEndP );
  
	osg::Vec3 position_;
	double radius_;
	double incrementAngle_;

	osg::ref_ptr< osg::Geometry > cylGeometry_;
	osg::ref_ptr< osg::Vec3Array > cylVertices_;
	osg::ref_ptr< osg::Vec3Array > cylNormals_;

};

