///////////////////////////////////////////////////////////
//  DeviceManager.h
//  Implementation of the Class DeviceManager
//  Created on:      26-Dec-2013 4:18:15 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(EA_8204B80E_EF46_47df_8AB8_FC787EF1223C__INCLUDED_)
#define EA_8204B80E_EF46_47df_8AB8_FC787EF1223C__INCLUDED_

#include "Device.h"

/**
 * Create one thread per device and let it process tasks and when finished, add
 * and output task to Scheduler
 */
namespace pn2s
{

class DeviceManager
{
public:
	static vector<Device> _device; //TODO: Should be private

	static int CkeckAvailableDevices();

	// Distribute model between devices
	static Error_PN2S Allocate(vector<models::Model > &m, double dt);
	static void PrepareSolvers();
	static void Process();
	static void Close();

};
}
#endif // !defined(EA_8204B80E_EF46_47df_8AB8_FC787EF1223C__INCLUDED_)
