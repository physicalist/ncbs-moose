///////////////////////////////////////////////////////////
//  PN2S_DeviceManager.cpp
//  Implementation of the Class PN2S_DeviceManager
//  Created on:      26-Dec-2013 4:18:15 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_DeviceManager.h"
#include <cuda.h>
#include <cuda_runtime_api.h>

PN2S_DeviceManager::PN2S_DeviceManager(){
	_devices.clear();
}



PN2S_DeviceManager::~PN2S_DeviceManager(){

}

void PN2S_DeviceManager::Setup(){
	cudaDeviceReset();
}
