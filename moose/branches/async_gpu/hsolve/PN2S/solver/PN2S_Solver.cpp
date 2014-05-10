///////////////////////////////////////////////////////////
//  PN2S_Solver.cpp
//  Implementation of the Class PN2S_Solver
//  Created on:      26-Dec-2013 4:21:51 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_Solver.h"
#include <assert.h>

PN2S_Solver::PN2S_Solver()
{
	_dt = 1; //1ms
	solverPacks.clear();
	_modelToPackMap.clear();
}

PN2S_Solver::~PN2S_Solver()
{

}

hscError PN2S_Solver::Setup(double dt){
	_dt = dt;
	return  NO_ERROR;
}


hscError PN2S_Solver::PrepareSolver(vector<PN2SModel> &m){
	//Estimate size of modelpack
	solverPacks.resize(1);
	solverPacks[0].SetDt(_dt);

	// Prepare solver for each modelpack
	hscError res = solverPacks[0].PrepareSolver(m);

	//Assign keys to modelPack, to be able to find later
	for(vector<PN2SModel>::iterator it = m.begin(); it != m.end(); ++it) {
		_modelToPackMap[it->id] = &solverPacks[0];
	}
	return res;
}


PN2S_SolverData* PN2S_Solver::LocateDataByID(hscID_t id){
	return _modelToPackMap[id];
}
/**
 * <ul>
 * 	<li>H2D stream call</li>
 * 	<li>Kernel call</li>
 * 	<li>D2H stram call</li>
 * </ul>
 *
 * When the output is ready, send it to the output task list
 */
void PN2S_Solver::Process(PN2S_SolverData* data, PN2S_Device* d){
	hscError res = data->Process();
	assert(!res);
}

#ifdef DO_UNIT_TESTS
#include <cassert>

using namespace std;

void testPN2S_Solver()
{
	cout << "testPN2S_Solver" << endl <<flush;
}
#endif


