///////////////////////////////////////////////////////////
//  Model.cpp
//  Implementation of the Class Model
//  Created on:      26-Dec-2013 9:48:38 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "Model.h"
#include "HHChannel.h"
#include "Compartment.h"

#include <typeinfo>

using namespace pn2s::models;


Model::Model(uint _id) :
		id(_id) {

	compts.clear();
	hhChannels.clear();
}

Model::Model(Matrix m,uint _id)
{
	id = _id ;
	matrix = m;
}

Model::~Model() {

}

