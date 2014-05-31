///////////////////////////////////////////////////////////
//  SolverData.cpp
//  Implementation of the Class SolverData
//  Created on:      27-Dec-2013 7:29:01 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "ModelPack.h"
#include "../headers.h"
#include <assert.h>

using namespace pn2s;
template <typename T, int arch>
ModelPack<T,arch>::ModelPack(): _dt(1){

}

template <typename T, int arch>
ModelPack<T,arch>::~ModelPack(){

}

template <typename T, int arch>
Error_PN2S ModelPack<T,arch>::Reinit(vector<models::Model<T> > &net){
	Error_PN2S res = Error_PN2S::NO_ERROR;

	_analyzer.ImportNetwork(net);

	res = _compsSolver.PrepareSolver(net, _analyzer);
	assert(res==Error_PN2S::NO_ERROR);
//	res = _channelsSolver.PrepareSolver(net, _analyzer);
//	assert(res==Error_PN2S::NO_ERROR);

	return  res;
}


template <typename T, int arch>
Error_PN2S ModelPack<T,arch>::Process()
{
	//For each model in the pack, executes the Process()
	_compsSolver.Process();
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S ModelPack<T,arch>::Output()
{
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S ModelPack<T,arch>::Input()
{
	return Error_PN2S::NO_ERROR;
}

template class ModelPack<double, ARCH_SM30>;


