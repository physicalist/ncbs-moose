///////////////////////////////////////////////////////////
//  PN2SModel_HHChannel.cpp
//  Implementation of the Class PN2SModel_HHChannel
//  Created on:      26-Dec-2013 4:20:54 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2SModel_HHChannel.h"
#include "../../PN2S.h"

template <typename T, int arch>
PN2SModel_HHChannel<T,arch>::PN2SModel_HHChannel(){
}

template <typename T, int arch>
PN2SModel_HHChannel<T,arch>::~PN2SModel_HHChannel(){
}

template class PN2SModel_HHChannel<double, ARCH_SM30>;
