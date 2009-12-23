/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <osg/Vec3>
#include <osg/ref_ptr>
#include <osg/Geometry>

#include <vector>

#include "GLCompartmentTri.h"
#include "GLCompartmentTriData.h"

GLCompartmentTri::GLCompartmentTri( osg::Vec3 corner1, osg::Vec3 corner2, osg::Vec3 corner3 )
	:
	corner1_( corner1 ),
	corner2_( corner2 ),
	corner3_( corner3 )
{
	init();
}

GLCompartmentTri::GLCompartmentTri( const GLCompartmentTriData& data )
	:
	corner1_( data.corner1[0], data.corner1[1], data.corner1[2] ),
	corner2_( data.corner2[0], data.corner2[1], data.corner2[2] ),
	corner3_( data.corner3[0], data.corner3[1], data.corner3[2] )
{
	init();
}

GLCompartmentTri::~GLCompartmentTri()
{
	cylGeometry_ = NULL;
	cylVertices_ = NULL;
	cylNormals_ = NULL;
}

void GLCompartmentTri::init()
{
	cylGeometry_ = new osg::Geometry;
	cylVertices_ = new osg::Vec3Array;
	cylNormals_ = new osg::Vec3Array;

	constructGeometry();

	cylGeometry_->setVertexArray( cylVertices_ );
	cylGeometry_->setNormalArray( cylNormals_ );
	cylGeometry_->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE_SET );
}

void GLCompartmentTri::constructGeometry()
{
	cylVertices_->push_back( corner1_ );
	cylVertices_->push_back( corner2_ );
	cylVertices_->push_back( corner3_ );

	osg::DrawElementsUInt* cylFaces = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

	cylFaces->push_back( 0 );
	cylFaces->push_back( 1 );
	cylFaces->push_back( 2 );

	cylGeometry_->addPrimitiveSet( cylFaces );

	cylNormals_->push_back(makeNormal( ( *cylVertices_ )[0],
					   ( *cylVertices_ )[1],
					   ( *cylVertices_ )[2] ));
}

osg::ref_ptr< osg::Geometry > GLCompartmentTri::getGeometry()
{
	return cylGeometry_;
}

CompartmentType GLCompartmentTri::getCompartmentType()
{
	return COMP_TRI;
}

void GLCompartmentTri::setColor( osg::Vec4 color )
{
	osg::Vec4Array* colors_ = new osg::Vec4Array;

	colors_->push_back( color );

	cylGeometry_->setColorArray( colors_ );
	cylGeometry_->setColorBinding( osg::Geometry::BIND_OVERALL );
}
