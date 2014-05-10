///////////////////////////////////////////////////////////
//  PN2S_Device.cpp
//  Implementation of the Class PN2S_Device
//  Created on:      26-Dec-2013 4:18:01 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#include "PN2S_Device.h"
#include "Definitions.h"
#include <cuda.h>
#include <cuda_runtime.h>

PN2S_Device::PN2S_Device(int _id): id(_id){
	cudaDeviceReset();
}

PN2S_Device::~PN2S_Device(){
	cudaDeviceReset();
}


int PN2S_Device::GetNumberOfActiveDevices(){
	 int device_count = 0;
	 cudaGetDeviceCount(&device_count);
	 return device_count;
}

hscError PN2S_Device::SelectDevice(){
	cudaDeviceProp deviceProp;
	cudaSetDevice(id);
	cudaGetDeviceProperties(&deviceProp, id);
	//TODO: Set configurations in device
	return  NO_ERROR;
}
