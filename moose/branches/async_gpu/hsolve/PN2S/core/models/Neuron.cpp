///////////////////////////////////////////////////////////
//  Model.cpp
//  Implementation of the Class Model
//  Created on:      26-Dec-2013 9:48:38 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "Neuron.h"
//#include "HHChannel.h"
#include "Compartment.h"
#include <typeinfo>

using namespace pn2s::models;

extern CompartmentVector compt_vec;

Neuron::Neuron(unsigned int _gid, int idx)
		:gid(_gid), _compt_base(idx), _compt_size(0)
{
}

//Copy constractor
Neuron::Neuron( const Neuron& other )
{
}
Neuron& Neuron::operator=(Neuron arg) {
	return *this;
}

CompartmentVector& Neuron::compt(){
	return compt_vec;
}

