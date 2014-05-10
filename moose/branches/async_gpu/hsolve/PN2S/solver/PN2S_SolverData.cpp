///////////////////////////////////////////////////////////
//  PN2S_SolverData.cpp
//  Implementation of the Class PN2S_SolverData
//  Created on:      27-Dec-2013 7:29:01 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_SolverData.h"
#include "../Definitions.h"
#include <assert.h>


PN2S_SolverData::PN2S_SolverData(){

}

PN2S_SolverData::~PN2S_SolverData(){

}

hscError PN2S_SolverData::PrepareSolver(vector<PN2SModel > &net){
	hscError res = NO_ERROR;

	_analyzer.ImportNetwork(net);

	res = _compsSolver.PrepareSolver(net, _analyzer);
	assert(res==NO_ERROR);
	res = _channelSolver.PrepareSolver(net, _analyzer);
	assert(res==NO_ERROR);
	return  res;
}

hscError PN2S_SolverData::Process()
{
	_compsSolver.Process();
	return NO_ERROR;
}
