/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef GLCELLRESETDATA_H
#define GLCELLRESETDATA_H

struct GLcellResetData
{
	std::string pathName;
	double vScale;
	double bgcolorRed;
	double bgcolorGreen;
	double bgcolorBlue;
	std::vector< GLcellProcData > renderListCompartmentData;

	template< typename Archive > 
	void serialize( Archive& ar, const unsigned int version )
	{
		ar & pathName;
		ar & vScale;
		ar & bgcolorRed;
		ar & bgcolorGreen;
		ar & bgcolorBlue;
		ar & renderListCompartmentData;
	}
};

#endif // GLCELLRESETDATA_H
