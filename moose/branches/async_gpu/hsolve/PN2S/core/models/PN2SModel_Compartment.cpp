///////////////////////////////////////////////////////////
//  PN2SModel_Compartment.cpp
//  Implementation of the Class PN2SModel_Compartment
//  Created on:      27-Dec-2013 9:35:18 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#include "PN2SModel_Compartment.h"

template <typename T, int arch>
PN2SModel_Compartment<T,arch>::PN2SModel_Compartment(): gid(-1){
}

template <typename T, int arch>
PN2SModel_Compartment<T,arch>::~PN2SModel_Compartment(){

}

//Copy constractor
template <typename T, int arch>
PN2SModel_Compartment<T,arch>::PN2SModel_Compartment( const PN2SModel_Compartment& other )
{
	children.assign(other.children.begin(), other.children.end());
	gid = other.gid;
}

template class PN2SModel_Compartment<double, ARCH_SM30>;
