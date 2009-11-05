/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef GLVIEWRESETDATA_H
#define GLVIEWRESETDATA_H

struct GLviewShapeResetData
{
	unsigned int id;
	std::string pathName;
	double x, y, z;

	template< typename Archive >
	void serialize( Archive& ar, const unsigned int version )
	{
		ar & id;
		ar & pathName;
		ar & x;
		ar & y;
		ar & z;
	}	     
};

struct GLviewResetData
{
	double bgcolorRed;
	double bgcolorGreen;
	double bgcolorBlue;
	std::string pathName;

	// this will be the maximum size (absolute value) of
	// our elements along any dimension
	double maxsize;
	
	std::vector< GLviewShapeResetData > shapes;

	template< typename Archive >
	void serialize( Archive& ar, const unsigned int version )
	{
		ar & bgcolorRed;
		ar & bgcolorGreen;
		ar & bgcolorBlue;
		ar & pathName;
		ar & maxsize;
		ar & shapes;
	}
};

#endif // GLVIEWRESETDATA_H
