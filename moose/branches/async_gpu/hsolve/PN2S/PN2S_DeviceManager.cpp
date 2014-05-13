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

	int device_count = 0;
	cudaGetDeviceCount(&device_count);

	for(int i =0; i<device_count; i++)
	{
		PN2S_Device d(i);
		_devices.push_back(d);
	}
}

PN2S_DeviceManager::~PN2S_DeviceManager(){

}

hscError PN2S_Device::SelectDevice(){
	cudaDeviceProp deviceProp;
	cudaSetDevice(id);
	//TODO: Set configurations in device
//	cudaGetDeviceProperties(&deviceProp, id);
	return  NO_ERROR;
}

hscError PN2S_DeviceManager::Setup(vector<PN2SModel> &m, double dt){
	cudaDeviceReset();

	//TODO: Devide model for more than one devices
	for(vector<PN2S_Device>::iterator it = _devices.begin(); it != _devices.end(); ++it)
	{
		it->PrepareSolver(m, dt);
	}

	return NO_ERROR;
}

void PN2S_DeviceManager::Process()
{
	for(vector<PN2S_Device>::iterator it = _devices.begin(); it != _devices.end(); ++it)
	{
		it->Process();
	}
}

