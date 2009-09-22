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

class GLCompartmentCylinder : public GLCompartment
{
public:
	GLCompartmentCylinder( osg::Vec3 position, osg::Quat quatRotation, double height, double radius, double incrementAngle );
	~GLCompartmentCylinder();

	osg::ref_ptr< osg::Geometry > getGeometry();

	void addHemisphericalCap( bool leftEndP );
	bool addHalfJointToNeighbour( GLCompartmentCylinder* neighbour );

	void setColor( osg::Vec4 color );

	int getCompartmentType();

	bool isLeftEndClosed;
	bool isRightEndClosed;

	osg::ref_ptr< osg::Vec3Array > ringRight;
	osg::ref_ptr< osg::Vec3Array > ringLeft;

private:
	osg::Vec3 makeNormal( const osg::Vec3& P1, const osg::Vec3& P2, const osg::Vec3& P3 );
  
	osg::Vec3 position_;
	osg::Quat quatRotation_;
	double height_;
	double radius_;
	double incrementAngle_;

	osg::ref_ptr< osg::Geometry > cylGeometry_;
	osg::ref_ptr< osg::Vec3Array > cylVertices_;
	osg::ref_ptr< osg::Vec3Array > cylNormals_;

};

