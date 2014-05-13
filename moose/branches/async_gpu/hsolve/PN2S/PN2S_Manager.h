///////////////////////////////////////////////////////////
//  PN2S_Manager.h
//  Implementation of the Class PN2S_Manager
//  Created on:      26-Dec-2013 4:18:17 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A1182FDE9_1428_42fc_B1C4_EB304C128113__INCLUDED_)
#define A1182FDE9_1428_42fc_B1C4_EB304C128113__INCLUDED_

#include "Definitions.h"
#include "core/PN2SModel.h"
#include "PN2S_DeviceManager.h"

/**
 * This class has a thread to look at scheduler for new tasks and send them to
 * devices.
 */
class PN2S_Manager
{
public:
	static hscError Setup(double dt);
	static hscError Reinit();
	static bool IsInitialized();
	static void InsertModel(PN2SModel &m);
	static hscError PrepareSolver();
	static hscError Process();
	static hscError AddInputTask(uint id);

private:


};
#endif // !defined(A1182FDE9_1428_42fc_B1C4_EB304C128113__INCLUDED_)
