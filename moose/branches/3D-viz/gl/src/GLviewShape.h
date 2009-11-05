/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef GLVIEWSHAPE_H
#define GLVIEWSHAPE_H

class GLviewShape
{
 public:
	GLviewShape( unsigned int id, std::string pathName, 
		     double x, double y, double z,
		     double xlen, double ylen, double zlen );
	osg::ref_ptr< osg::Geode > getGeode();
	void setColor( osg::Vec4 color );
	void move( double xoffset, double yoffset, double zoffset );
	void resize( double xlen, double ylen, double zlen );
	// void setIcon( std::string iconFileName ); // TODO

 private:
	unsigned int id_;
	std::string pathName_;
	double x_, y_, z_;
	double xlen_, ylen_, zlen_;
	double xoffset_, yoffset_, zoffset_;
	
	std::string iconFileName_;

	osg::ref_ptr< osg::Geode > geode_;
	osg::ref_ptr< osg::ShapeDrawable > drawable_;
	osg::ref_ptr< osg::Box > box_;
};

#endif // GLVIEWSHAPE_H
