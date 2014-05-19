///////////////////////////////////////////////////////////
//  PN2S_Manager.h
//  Implementation of the Class PN2S_Manager
//  Created on:      26-Dec-2013 4:18:17 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A1182FDE9_1428_42fc_B1C4_EB304C128113__INCLUDED_)
#define A1182FDE9_1428_42fc_B1C4_EB304C128113__INCLUDED_

#include "PN2S.h"
#include "core/models/PN2SModel.h"
#include "PN2S_DeviceManager.h"

/**
 * This class has a thread to look at scheduler for new tasks and send them to
 * devices.
 */
class PN2S_Manager
{
public:
	static Error_PN2S Setup(double dt);
	static Error_PN2S Reinit();
	static bool IsInitialized();
	static void InsertModel(PN2SModel<CURRENT_TYPE, CURRENT_ARCH> &m);
	static Error_PN2S PrepareSolver();
	static Error_PN2S Process();
	static Error_PN2S AddInputTask(uint id);

private:


};
#endif // !defined(A1182FDE9_1428_42fc_B1C4_EB304C128113__INCLUDED_)
