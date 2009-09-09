#ifndef GEOMETRYDATA_H
#define GEOMETRYDATA_H

struct GeometryData
{
	std::string pathName;
	std::vector< GLcellCompartment > renderListGLcellCompartments;

	template< typename Archive > 
	void serialize( Archive& ar, const unsigned int version )
	{
		ar & pathName;
		ar & renderListGLcellCompartments;
	}
};

#endif // GEOMETRYDATA_H
