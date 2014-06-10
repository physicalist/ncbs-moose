///////////////////////////////////////////////////////////
//  Model.cpp
//  Implementation of the Class Model
//  Created on:      26-Dec-2013 9:48:38 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "NeuronVector.h"
#include <typeinfo>

using namespace pn2s::models;

CompartmentVector compt_vec;

NeuronVector::~NeuronVector(){}

Neuron* NeuronVector::Create(int gid)
{
	Neuron n(gid, 0);//_compt_base+_compt_size);
//	_compt_size++;

	neurons.push_back(n);
	return &(*neurons.end());
}

Neuron & NeuronVector::operator[](int i)
{
	return neurons[i];
}

