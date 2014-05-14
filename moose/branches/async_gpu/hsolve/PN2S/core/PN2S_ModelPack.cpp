///////////////////////////////////////////////////////////
//  PN2S_SolverData.cpp
//  Implementation of the Class PN2S_SolverData
//  Created on:      27-Dec-2013 7:29:01 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_ModelPack.h"
#include "../Definitions.h"
#include <assert.h>

template <typename T, int arch>
PN2S_ModelPack<T,arch>::PN2S_ModelPack(): _dt(1){

}

template <typename T, int arch>
PN2S_ModelPack<T,arch>::~PN2S_ModelPack(){

}

template <typename T, int arch>
hscError PN2S_ModelPack<T,arch>::PrepareSolver(vector<PN2SModel<T,arch> > &net){
	hscError res = NO_ERROR;

	_analyzer.ImportNetwork(net);

	res = _compsSolver.PrepareSolver(net, _analyzer);
	assert(res==NO_ERROR);
	res = _channelSolver.PrepareSolver(net, _analyzer);
	assert(res==NO_ERROR);
	return  res;
}


template <typename T, int arch>
hscError PN2S_ModelPack<T,arch>::Process()
{
	//For each model in the pack, executes the Process()
	_compsSolver.Process();
	return NO_ERROR;
}

template <typename T, int arch>
hscError PN2S_ModelPack<T,arch>::AddField()
{

	return NO_ERROR;
}

template class PN2S_ModelPack<double, ARCH_SM30>;


