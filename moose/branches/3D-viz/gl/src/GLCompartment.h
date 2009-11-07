/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef GLCOMPARTMENT_H
#define GLCOMPARTMENT_H

enum COMPARTMENT_TYPE
{
	COMP_CYLINDER,
	COMP_SPHERE
};

class GLCompartment
{
 public:
	virtual osg::ref_ptr< osg::Geometry > getGeometry() = 0;
	virtual void setColor( osg::Vec4 color ) = 0;
	
	virtual int getCompartmentType() = 0;
	
	virtual ~GLCompartment() {}

 protected:
	virtual osg::Vec3 rotateTranslatePoint( osg::Vec3 position, osg::Quat& quatRotation, osg::Vec3& translation );
	virtual osg::Vec3 makeNormal( const osg::Vec3& P1, const osg::Vec3& P2, const osg::Vec3& P3 );
};

#endif // GLCOMPARTMENT_H
