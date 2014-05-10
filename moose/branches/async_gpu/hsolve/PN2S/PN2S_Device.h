///////////////////////////////////////////////////////////
//  PN2S_Device.h
//  Implementation of the Class PN2S_Device
//  Created on:      26-Dec-2013 4:18:01 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#if !defined(AB1F38D16_FE78_4893_BC39_356BCBBCF345__INCLUDED_)
#define AB1F38D16_FE78_4893_BC39_356BCBBCF345__INCLUDED_
#include "Definitions.h"

class PN2S_Device
{

public:
	PN2S_Device(int _id);
	virtual ~PN2S_Device();
	static int GetNumberOfActiveDevices();
	hscError SelectDevice();
	int id;
};
#endif // !defined(AB1F38D16_FE78_4893_BC39_356BCBBCF345__INCLUDED_)
