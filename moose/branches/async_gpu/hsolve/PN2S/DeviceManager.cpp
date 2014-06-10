///////////////////////////////////////////////////////////
//  DeviceManager.cpp
//  Implementation of the Class DeviceManager
//  Created on:      26-Dec-2013 4:18:15 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "DeviceManager.h"
#include <cuda.h>
#include <cuda_runtime_api.h>

using namespace pn2s;
DeviceManager::DeviceManager(){
	_devices.clear();

	int device_count = 0;
	cudaGetDeviceCount(&device_count);

	for(int i =0; i<device_count; i++)
	{
		Device d(i);
		_devices.push_back(d);
	}
}

DeviceManager::~DeviceManager(){

}

Error_PN2S DeviceManager::SelectDevice(int id){
	cudaDeviceProp deviceProp;
	cudaSetDevice(id);
	//TODO: Set configurations in device
//	cudaGetDeviceProperties(&deviceProp, id);
	return  Error_PN2S::NO_ERROR;
}

Error_PN2S DeviceManager::Reinit(vector<models::Model > &m, double dt){
	cudaDeviceReset();

	//TODO: Add Multidevice
	_devices[0].Reinit(m, dt);
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

void DeviceManager::Process()
{
	//TODO: Each device should get its own pack
	for(vector<Device>::iterator device = _devices.begin(); device != _devices.end(); ++device)
	{
		device->Process();
	}
}

