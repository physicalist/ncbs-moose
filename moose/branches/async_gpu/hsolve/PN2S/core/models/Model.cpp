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


template <typename T>
Model<T>::Model(uint _id) :
		id(_id) {

	compts.clear();
	hhChannels.clear();
}

template <typename T>
Model<T>::Model(Matrix m,uint _id)
{
	id = _id ;
	matrix = m;
}

template <typename T>
Model<T>::~Model() {

}

template class Model<double>;
