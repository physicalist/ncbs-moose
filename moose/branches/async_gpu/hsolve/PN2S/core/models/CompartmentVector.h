#pragma once

#include "../headers.h"
#include "Compartment.h"

namespace pn2s
{
namespace models
{

class CompartmentVector
{
	static vector<Compartment> compt;
public:
	CompartmentVector() {}
	virtual ~CompartmentVector(){}


	Compartment* Create(int gid);
//	typename Compartment::itr Create(int gid);
//	Compartment &operator[](int i);
};

}
}
