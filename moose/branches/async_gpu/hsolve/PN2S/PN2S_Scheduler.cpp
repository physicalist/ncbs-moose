///////////////////////////////////////////////////////////
//  PN2S_Scheduler.cpp
//  Implementation of the Class PN2S_Scheduler
//  Created on:      26-Dec-2013 4:19:23 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_Scheduler.h"




PN2S_Scheduler::PN2S_Scheduler(){

}


PN2S_Scheduler::~PN2S_Scheduler(){

}


hscError PN2S_Scheduler::AddInputTask(PN2S_TaskInfo &task){
	_inputTasks.push_front(task);
	return  NO_ERROR;
}


PN2S_TaskInfo* PN2S_Scheduler::GetInputTask(){
	PN2S_TaskInfo* res= &_inputTasks[0];
	_inputTasks.pop_back();
	return res;
}


/**
 * This is the entry point of Process routine.
 * According to the ID it will find ModelPack corresponding to the model and
 * create a TaskInfo for that.
 * Also Read data from Shell for all Models at ModelPack and at the end add the
 * TaskInfo to _scheduleList
 * 
 * If Maganer is not started processing, Start it.
 */
void PN2S_Scheduler::Process(){

}


hscError PN2S_Scheduler::getValuesFromShell(){

	return  NO_ERROR;
}


hscError PN2S_Scheduler::AddOutputTask(){

	return  NO_ERROR;
}


PN2S_TaskInfo* PN2S_Scheduler::GetOutputTask(){

	return  NULL;
}
