///////////////////////////////////////////////////////////
//  PN2SModel.cpp
//  Implementation of the Class PN2SModel
//  Created on:      26-Dec-2013 9:48:38 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2SModel.h"
#include "PN2SModel_HHChannel.h"
#include "PN2SModel_Compartment.h"

#include <typeinfo>

template <typename T, int arch>
PN2SModel<T,arch>::PN2SModel(uint _id) :
		id(_id) {

	compts.clear();
	hhChannels.clear();
}

template <typename T, int arch>
PN2SModel<T,arch>::~PN2SModel() {

}

template class PN2SModel<double, ARCH_SM30>;
