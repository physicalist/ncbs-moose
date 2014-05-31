///////////////////////////////////////////////////////////
//  Compartment.cpp
//  Implementation of the Class Compartment
//  Created on:      27-Dec-2013 9:35:18 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#include "Compartment.h"

using namespace pn2s::models;

template <typename T>
Compartment<T>::Compartment(): gid(-1){
}

template <typename T>
Compartment<T>::~Compartment(){

}

//Copy constractor
template <typename T>
Compartment<T>::Compartment( const Compartment& other )
{
	children.assign(other.children.begin(), other.children.end());
	gid = other.gid;
}

template class Compartment<double>;
