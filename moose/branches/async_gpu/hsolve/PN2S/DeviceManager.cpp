///////////////////////////////////////////////////////////
//  DeviceManager.cpp
//  Implementation of the Class DeviceManager
//  Created on:      26-Dec-2013 4:18:15 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "DeviceManager.h"
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cuda_profiler_api.h>

using namespace pn2s;

vector<Device> DeviceManager::_device;
int DeviceManager::CkeckAvailableDevices(){
	cudaProfilerStop();
	_device.clear();

	int device_count = 0; //TODO: get value from device
	cudaDeviceReset();
	cudaGetDeviceCount(&device_count);

	for(int i =0; i<device_count; i++)
	{
		Device d(i);
		_device.push_back(d);
	}


	return device_count;
}

/**
 * This function, assign the model shapes into devices
 * and assign memory for PFields
 */

Error_PN2S DeviceManager::Allocate(vector<models::Model > &m, double dt){

	//TODO: Add Multidevice
	int32_t address = 0;
	_device[0].GenerateModelPacks(dt, &m[0],(size_t)0,(size_t)m.size(),address);
//	int numDevice = _devices.size();
//	int numModel  = m.size();
//
//	if(numDevice > numModel)
//		numDevice = numModel;
//
//	vector<models<TYPE_, CURRENT_ARCH> >::iterator it = m.begin();
//
//	for(int i = 0; i< numDevice;i++)
//	{
//		vector<models<TYPE_, CURRENT_ARCH> > subModel (it, it + numModel/numDevice);
//		_devices[i].Reinit(subModel, dt);
//
//		it += numModel/numDevice+1;
//	}

	return Error_PN2S::NO_ERROR;
}

void DeviceManager::PrepareSolvers()
{
	for(vector<Device>::iterator device = _device.begin(); device != _device.end(); ++device)
	{
		device->PrepareSolvers();
	}
	cudaProfilerStart();
}

void DeviceManager::Process()
{

	//TODO: Each device should get its own pack
	for(vector<Device>::iterator device = _device.begin(); device != _device.end(); ++device)
		device->Process();

}

void DeviceManager::Close()
{
	cudaProfilerStop();
	for(vector<Device>::iterator device = _device.begin(); device != _device.end(); ++device)
		device->Destroy();
}

