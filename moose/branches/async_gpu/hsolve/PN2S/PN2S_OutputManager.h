///////////////////////////////////////////////////////////
//  PN2S_OutputManager.h
//  Implementation of the Class PN2S_OutputManager
//  Created on:      26-Dec-2013 4:18:18 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(EA_7219F77E_D358_4d9b_A477_5363274F9118__INCLUDED_)
#define EA_7219F77E_D358_4d9b_A477_5363274F9118__INCLUDED_

#include "Definitions.h"
#include "PN2S_TaskInfo.h"
#include "PN2S_OutputWriter.h"
#include "PN2S_Scheduler.h"

/**
 * This class is responsible to get output tasks from scheduler and write them to
 * output. For example send the result to the Shell.
 * It contains a thread that is wait on output list at Scheduler 
 */
class PN2S_OutputManager
{

public:
	PN2S_OutputManager();
	virtual ~PN2S_OutputManager();
	PN2S_OutputWriter *m_PN2S_OutputWriter;
	PN2S_Scheduler *m_PN2S_Scheduler;

//	hscError_t SendDataToShell(PN2S_TaskInfo task);

};
#endif // !defined(EA_7219F77E_D358_4d9b_A477_5363274F9118__INCLUDED_)
