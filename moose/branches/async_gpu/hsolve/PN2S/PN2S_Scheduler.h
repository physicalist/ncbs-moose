///////////////////////////////////////////////////////////
//  PN2S_Scheduler.h
//  Implementation of the Class PN2S_Scheduler
//  Created on:      26-Dec-2013 4:19:23 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(AE33FA2BC_2680_47eb_9858_1D1498C6FAE0__INCLUDED_)
#define AE33FA2BC_2680_47eb_9858_1D1498C6FAE0__INCLUDED_

#include "PN2S_TaskInfo.h"

class PN2S_Scheduler
{

public:
	deque<PN2S_TaskInfo> _inputTasks; //FIFO queue
	deque<PN2S_TaskInfo> _outputTasks; //FIFO queue

	PN2S_Scheduler();
	virtual ~PN2S_Scheduler();
	hscError AddInputTask(PN2S_TaskInfo &task);
	PN2S_TaskInfo* GetInputTask();
	void Process();
	hscError AddOutputTask();
	PN2S_TaskInfo* GetOutputTask();

private:
	hscError getValuesFromShell();

};
#endif // !defined(AE33FA2BC_2680_47eb_9858_1D1498C6FAE0__INCLUDED_)
