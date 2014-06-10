///////////////////////////////////////////////////////////
//  Manager.h
//  Implementation of the Class Manager
//  Created on:      26-Dec-2013 4:18:17 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A1182FDE9_1428_42fc_B1C4_EB304C128113__INCLUDED_)
#define A1182FDE9_1428_42fc_B1C4_EB304C128113__INCLUDED_

#include "headers.h"
#include "core/models/Model.h"
#include "DeviceManager.h"

/**
 * This class has a thread to look at scheduler for new tasks and send them to
 * devices.
 */

namespace pn2s
{

class Manager
{
public:
	static Error_PN2S Setup(double dt);
	static Error_PN2S Reinit();
	static bool IsInitialized();
	static void InsertModel(models::Model &m);
	static Error_PN2S Process();
	static Error_PN2S AddInputTask(uint id);

private:


};
}
#endif // !defined(A1182FDE9_1428_42fc_B1C4_EB304C128113__INCLUDED_)
