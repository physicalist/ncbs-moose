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
#include <algorithm>
#include <math.h>

#include "GLCompartmentSphere.h"

GLCompartmentSphere::GLCompartmentSphere( osg::Vec3 position, double radius, double incrementAngle )
	:
	position_( position ),
	radius_( radius ),
	incrementAngle_( incrementAngle )
{ 
	cylGeometry_ = new osg::Geometry;
	cylVertices_ = new osg::Vec3Array;
	cylNormals_ = new osg::Vec3Array;

	addHemisphericalCap( true );
	addHemisphericalCap( false ); // two hemi-spheres make a sphere
	
	cylGeometry_->setVertexArray( cylVertices_ );
	cylGeometry_->setNormalArray( cylNormals_ );
	cylGeometry_->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE_SET );
}

GLCompartmentSphere::~GLCompartmentSphere()
{
	cylGeometry_ = NULL;
	cylVertices_ = NULL;
	cylNormals_ = NULL;
}

osg::ref_ptr< osg::Geometry > GLCompartmentSphere::getGeometry()
{
	return cylGeometry_;
}

int GLCompartmentSphere::getCompartmentType()
{
	return SPHERE;
}

void GLCompartmentSphere::setColor( osg::Vec4 color )
{
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( color );

	cylGeometry_->setColorArray( colors );
	cylGeometry_->setColorBinding( osg::Geometry::BIND_OVERALL );
}

void GLCompartmentSphere::addHemisphericalCap( bool leftEndP )
{
	int oldSizeCylVertices = cylVertices_->size();

	std::vector< double > angles;
	for ( double i = 0; i <= 360 - incrementAngle_; i += incrementAngle_ )
		angles.push_back( 2*M_PI*i / 360 );
	angles.push_back( 0 );

	double neighbourSign;
	if ( leftEndP )
	{
		neighbourSign = -1;
	}
	else
	{
		neighbourSign = 1;
	}

	// add vertex at tip first
	cylVertices_->push_back( osg::Vec3( position_[0],
					    position_[1],
					    position_[2] + neighbourSign * radius_ ) );
	
	for (int i = 0; i < angles.size()-1; ++i)
	{
		osg::DrawElementsUInt* cylFaces = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

		cylVertices_->push_back( osg::Vec3( position_[0] + radius_ * cos( angles[i] ),
						    position_[1] + radius_ * sin( angles[i] ),
						    position_[2] ) );
		cylVertices_->push_back( osg::Vec3( position_[0] + radius_ * cos( angles[i+1] ),
						    position_[1] + radius_ * sin( angles[i+1] ),
						    position_[2] ) );

		int j;
		for ( j = 1; j <= 9; ++j )
		{
	  
			cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );

			cylVertices_->push_back( osg::Vec3( position_[0] + radius_ * sin( acos(j*0.1) ) * cos( angles[i] ),
							    position_[1] + radius_ * sin( acos(j*0.1) ) * sin( angles[i] ),
							    position_[2] + neighbourSign * ( (j*0.1) * radius_ ) ) );
			cylVertices_->push_back( osg::Vec3( position_[0] + radius_ * sin( acos(j*0.1) ) * cos( angles[i+1] ),
							    position_[1] + radius_ * sin( acos(j*0.1) ) * sin( angles[i+1] ),
							    position_[2] + neighbourSign * ( (j*0.1) * radius_ ) ) );

			cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 1 + 2*(j-1) ); // 20 == 2 + 9*2 ; vertices on middle ring + two vertices per face added
			cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 0 + 2*(j-1) );
			cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 2 + 2*(j-1) );
			cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 3 + 2*(j-1) );

			cylGeometry_->addPrimitiveSet( cylFaces );

			osg::Vec3 normal = makeNormal( ( *cylVertices_ )[oldSizeCylVertices + 1 + i*20 + 1 + 2*(j-1)],
						       ( *cylVertices_ )[oldSizeCylVertices + 1 + i*20 + 0 + 2*(j-1)],
						       ( *cylVertices_ )[oldSizeCylVertices + 1 + i*20 + 2 + 2*(j-1)] );
			cylNormals_->push_back(osg::Vec3( normal[0] * -1 * neighbourSign,
							  normal[1] * -1 * neighbourSign,
							  normal[2] * -1 * neighbourSign ) );
	
			cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0);


		}

		j = 9;
	
		cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
		cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 3 + 2*(j-1) );
		cylFaces->push_back( oldSizeCylVertices + 1 + i*20 + 2 + 2*(j-1) );
		cylFaces->push_back( oldSizeCylVertices );

		cylGeometry_->addPrimitiveSet( cylFaces );

		osg::Vec3 normal = makeNormal( ( *cylVertices_ )[oldSizeCylVertices + 1 + i*20 + 3 + 2*(j-1)],
					       ( *cylVertices_ )[oldSizeCylVertices + 1 + i*20 + 2 + 2*(j-1)],
					       ( *cylVertices_ )[oldSizeCylVertices] );
		cylNormals_->push_back( osg::Vec3( normal[0] * -1 * neighbourSign,
						   normal[1] * -1 * neighbourSign,
						   normal[2] * -1 * neighbourSign ) );
	}
}

