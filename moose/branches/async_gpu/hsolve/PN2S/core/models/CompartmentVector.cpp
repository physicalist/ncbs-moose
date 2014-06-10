///////////////////////////////////////////////////////////
//  Compartment.cpp
//  Implementation of the Class Compartment
//  Created on:      27-Dec-2013 9:35:18 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#include "CompartmentVector.h"

using namespace pn2s::models;

vector<Compartment> CompartmentVector::compt;

//typename Compartment::itr CompartmentVector::Create(int gid)
Compartment* CompartmentVector::Create(int gid)
{
	Compartment c(gid);//_compt_base+_compt_size);
//	_compt_size++;
//	compt.resize(compt.size()+1);
	compt.push_back(c);
	return &(*compt.end());
}
