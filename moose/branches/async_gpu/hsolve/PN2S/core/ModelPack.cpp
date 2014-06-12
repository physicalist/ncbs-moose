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

ModelPack::ModelPack(): _dt(1), models(0){

}

ModelPack::~ModelPack(){

}

Error_PN2S ModelPack::Allocate(models::Model *m, models::ModelStatistic s, cudaStream_t st){
	stat = s;
	models = m;

	Error_PN2S res = Error_PN2S::NO_ERROR;
	res = _compsSolver.AllocateMemory(models,stat, st);
	assert(res==Error_PN2S::NO_ERROR);

	return res;
}

Error_PN2S ModelPack::PrepareSolvers(){
	Error_PN2S res = Error_PN2S::NO_ERROR;

	res = _compsSolver.PrepareSolver();
	assert(res==Error_PN2S::NO_ERROR);
//	res = _channelsSolver.PrepareSolver(net, _analyzer);
//	assert(res==Error_PN2S::NO_ERROR);

	return  res;
}


void ModelPack::Input()
{
	//For each model in the pack, executes the Process()
	_compsSolver.Input();
}

void ModelPack::Process()
{
	_compsSolver.Process();
}

void ModelPack::Output()
{
	_compsSolver.Output();
}



