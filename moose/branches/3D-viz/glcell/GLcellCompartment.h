#ifndef GLCELLCOMPARTMENT_H
#define GLCELLCOMPARTMENT_H

struct GLcellCompartment
{
	double diameter;
	double length;
	double x0;
	double y0;
	double z0;
	double x;
	double y;
	double z;
	
	template< typename Archive > 
	void serialize( Archive& ar, const unsigned int version )
	{
		ar & diameter;
		ar & length;
		ar & x0;
		ar & y0;
		ar & z0;
		ar & x;
		ar & y;
		ar & z;
	}
};

#endif // GLCELLCOMPARTMENT_H

