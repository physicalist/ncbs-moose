///////////////////////////////////////////////////////////
//  PN2SModel_Compartment.cpp
//  Implementation of the Class PN2SModel_Compartment
//  Created on:      27-Dec-2013 9:35:18 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#include "PN2SModel_Compartment.h"
//#include "../Definitions.h"

PN2SModel_Compartment::PN2SModel_Compartment(){
}


PN2SModel_Compartment::~PN2SModel_Compartment(){

}

//Copy constractor
PN2SModel_Compartment::PN2SModel_Compartment( const PN2SModel_Compartment& other ) :
		Ra(other.Ra), Rm(other.Rm), Cm(other.Cm), Em(other.Em), initVm(other.initVm), Vm(other.Vm)
{
	children.assign(other.children.begin(), other.children.end());
}
