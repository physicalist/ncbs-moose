#ifndef COMPARTMENTDATA_H
#define COMPARTMENTDATA_H

struct CompartmentData
{
	std::string name;
	unsigned int id;
	std::string pathName;
	std::vector< unsigned int > vNeighbourIds;

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
		ar & name;
		ar & id;
		ar & pathName;
		ar & vNeighbourIds;
		
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

#endif // COMPARTMENTDATA_H

