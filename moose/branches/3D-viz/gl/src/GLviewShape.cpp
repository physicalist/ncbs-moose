/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <osg/Vec4>
#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Shape>

#include "GLviewShape.h"

GLviewShape::GLviewShape( unsigned int id, std::string pathName, 
			  double x, double y, double z,
			  double xlen, double ylen, double zlen )
	:
	xoffset_( 0.0 ),
	yoffset_( 0.0 ),
	zoffset_( 0.0 ),
	iconFileName_( "" )
{
	id_ = id;
	pathName_ = pathName;
	x_ = x; 
	y_ = y;
	z_ = z;
	xlen_ = xlen;
	ylen_ = ylen;
	zlen_ = zlen;

	box_ = new osg::Box( osg::Vec3( x_ + xlen_/2,
					y_ + ylen_/2,
					z_ + zlen_/2 ),
			     xlen_, ylen_, zlen_ );
	drawable_ = new osg::ShapeDrawable( box_ );
	geode_ = new osg::Geode();	
	geode_->addDrawable( drawable_ );
}

osg::ref_ptr< osg::Geode > GLviewShape::getGeode()
{
	return geode_;
}

void GLviewShape::setColor( osg::Vec4 color )
{
	drawable_->setColor( color );
}

void GLviewShape::move( double xoffset, double yoffset, double zoffset )
{
	xoffset_ = xoffset;
	yoffset_ = yoffset;
	zoffset_ = zoffset;

	box_->setCenter( osg::Vec3( x_ + xlen_/2 + xoffset_,
				   y_ + ylen_/2 + yoffset_,
				   z_ + zlen_/2 + zoffset_ ) );
}

void GLviewShape::resize( double xlen, double ylen, double zlen )
{
	xlen_ = xlen;
	ylen_ = ylen;
	zlen_ = zlen;
	
	box_->setHalfLengths( osg::Vec3( xlen_/2, ylen_/2, zlen_/2 ) );
}

