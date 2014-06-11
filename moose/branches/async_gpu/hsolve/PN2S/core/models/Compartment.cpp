///////////////////////////////////////////////////////////
//  Compartment.cpp
//  Implementation of the Class Compartment
//  Created on:      27-Dec-2013 9:35:18 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#include "Compartment.h"
#include "PField.h"

using namespace pn2s::models;

Compartment::Compartment(int _gid): gid(_gid){

}

Compartment::~Compartment(){

}

//Copy constractor
Compartment::Compartment( const Compartment& other )
{
	children.assign(other.children.begin(), other.children.end());
	gid = other.gid;
	location = other.location;
}
