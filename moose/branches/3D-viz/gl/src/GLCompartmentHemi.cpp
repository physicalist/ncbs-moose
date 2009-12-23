/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <osg/Vec3d>
#include <osg/ref_ptr>
#include <osg/Geometry>

#include <vector>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846             
#endif

#include "GLCompartmentHemi.h"
#include "GLCompartmentHemiData.h"

GLCompartmentHemi::GLCompartmentHemi( osg::Vec3 centre, osg::Vec3f orientation, double radius, double incrementAngle )
	:
	centre_( centre ),
	radius_( radius ),
	incrementAngle_( incrementAngle ),
	orientation_( orientation )
{ 
	init();
}

GLCompartmentHemi::GLCompartmentHemi( const GLCompartmentHemiData& data, double incrementAngle )
	:
	centre_( data.centre[0], data.centre[1], data.centre[2] ),
	radius_( data.radius ),
	incrementAngle_( incrementAngle ),
	orientation_( data.orientation[0], data.orientation[1], data.orientation[2] )
{
	init();
}

GLCompartmentHemi::~GLCompartmentHemi()
{
	cylGeometry_ = NULL;
	cylVertices_ = NULL;
	cylNormals_ = NULL;
}

osg::ref_ptr< osg::Geometry > GLCompartmentHemi::getGeometry()
{
	return cylGeometry_;
}

CompartmentType GLCompartmentHemi::getCompartmentType()
{
	return COMP_HEMISPHERE;
}

void GLCompartmentHemi::setColor( osg::Vec4 color )
{
	osg::Vec4Array* colors_ = new osg::Vec4Array;

	colors_->push_back( color );

	cylGeometry_->setColorArray( colors_ );
	cylGeometry_->setColorBinding( osg::Geometry::BIND_OVERALL );
}

void GLCompartmentHemi::init()
{
	cylGeometry_ = new osg::Geometry;
	cylVertices_ = new osg::Vec3Array;
	cylNormals_ = new osg::Vec3Array;

	constructGeometry();
	
	cylGeometry_->setVertexArray( cylVertices_ );
	cylGeometry_->setNormalArray( cylNormals_ );
	cylGeometry_->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE_SET );
}

void GLCompartmentHemi::constructGeometry()
{
	std::vector< double > angles;
	for ( double i = 0; i <= 360 - incrementAngle_; i += incrementAngle_ )
		angles.push_back( 2*M_PI*i / 360 );
	angles.push_back( 0 );
	
	// prepare quaternion to rotate each point according to specified orientation
	osg::Vec3f initial( 0.0f, 0.0f, 1.0f );
	initial.normalize();

	orientation_.normalize();

	osg::Vec3f axis = initial ^ orientation_;
	axis.normalize();
	osg::Quat quatRotation( acos( initial * orientation_ ), axis );

	// add vertex at tip first
	cylVertices_->push_back( quatRotation * osg::Vec3( centre_[0],
							   centre_[1],
							   centre_[2] + radius_ ) );
	
	for ( unsigned int i = 0; i < angles.size()-1; ++i)
	{
		osg::DrawElementsUInt* cylFaces = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

		cylVertices_->push_back( quatRotation * osg::Vec3( centre_[0] + radius_ * cos( angles[i] ),
								   centre_[1] + radius_ * sin( angles[i] ),
								   centre_[2] ) );
		cylVertices_->push_back( quatRotation * osg::Vec3( centre_[0] + radius_ * cos( angles[i+1] ),
								   centre_[1] + radius_ * sin( angles[i+1] ),
								   centre_[2] ) );

		unsigned int j;
		for ( j = 1; j <= 9; ++j )
		{
	  
			cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );

			cylVertices_->push_back( quatRotation * osg::Vec3( centre_[0] + radius_ * sin( acos(j*0.1) ) * cos( angles[i] ),
									   centre_[1] + radius_ * sin( acos(j*0.1) ) * sin( angles[i] ),
									   centre_[2] + ( (j*0.1) * radius_ ) ) );
			cylVertices_->push_back( quatRotation * osg::Vec3( centre_[0] + radius_ * sin( acos(j*0.1) ) * cos( angles[i+1] ),
									   centre_[1] + radius_ * sin( acos(j*0.1) ) * sin( angles[i+1] ),
									   centre_[2] + ( (j*0.1) * radius_ ) ) );

			cylFaces->push_back( 1 + i*20 + 1 + 2*(j-1) ); // 20 == 2 + 9*2 ; vertices on middle ring + two vertices per face added
			cylFaces->push_back( 1 + i*20 + 0 + 2*(j-1) );
			cylFaces->push_back( 1 + i*20 + 2 + 2*(j-1) );
			cylFaces->push_back( 1 + i*20 + 3 + 2*(j-1) );

			cylGeometry_->addPrimitiveSet( cylFaces );

			osg::Vec3 normal = makeNormal( ( *cylVertices_ )[1 + i*20 + 1 + 2*(j-1)],
						       ( *cylVertices_ )[1 + i*20 + 0 + 2*(j-1)],
						       ( *cylVertices_ )[1 + i*20 + 2 + 2*(j-1)] );
			cylNormals_->push_back(osg::Vec3( normal[0] * -1,
							  normal[1] * -1,
							  normal[2] * -1 ) );
	
			cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0);


		}

		j = 9;
	
		cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
		cylFaces->push_back( 1 + i*20 + 3 + 2*(j-1) );
		cylFaces->push_back( 1 + i*20 + 2 + 2*(j-1) );
		cylFaces->push_back( 0 );

		cylGeometry_->addPrimitiveSet( cylFaces );

		osg::Vec3 normal = makeNormal( ( *cylVertices_ )[1 + i*20 + 3 + 2*(j-1)],
					       ( *cylVertices_ )[1 + i*20 + 2 + 2*(j-1)],
					       ( *cylVertices_ )[0] );
		cylNormals_->push_back( osg::Vec3( normal[0] * -1,
						   normal[1] * -1,
						   normal[2] * -1 ) );
	}
}

