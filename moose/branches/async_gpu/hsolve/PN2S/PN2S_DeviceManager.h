///////////////////////////////////////////////////////////
//  PN2S_DeviceManager.h
//  Implementation of the Class PN2S_DeviceManager
//  Created on:      26-Dec-2013 4:18:15 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(EA_8204B80E_EF46_47df_8AB8_FC787EF1223C__INCLUDED_)
#define EA_8204B80E_EF46_47df_8AB8_FC787EF1223C__INCLUDED_

#include "PN2S_Device.h"

/**
 * Create one thread per device and let it process tasks and when finished, add
 * and output task to Scheduler
 */
class PN2S_DeviceManager
{

public:
	PN2S_DeviceManager();
	virtual ~PN2S_DeviceManager();
	Error_PN2S Reinit(vector<PN2SModel<CURRENT_TYPE, CURRENT_ARCH> > &m, double dt);
	void Process();
	vector<PN2S_Device> _devices; //TODO: Should be private
private:
	Error_PN2S SelectDevice(int id);
};
#endif // !defined(EA_8204B80E_EF46_47df_8AB8_FC787EF1223C__INCLUDED_)
