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
	modelPacks.clear();
	_modelToPackMap.clear();
}

PN2S_Solver::~PN2S_Solver()
{

}

hscError PN2S_Solver::PrepareSolver(vector<PN2SModel> &m,  double dt){
	_dt = dt;

	//TODO: Generate model packs
	modelPacks.resize(1);
	modelPacks[0].SetDt(_dt);

	//Prepare solver for each modelpack
	hscError res = modelPacks[0].PrepareSolver(m);

	//Assign keys to modelPack, to be able to find later
	for(vector<PN2SModel>::iterator it = m.begin(); it != m.end(); ++it) {
		_modelToPackMap[it->id] = &modelPacks[0];
	}
	return res;
}


PN2S_ModelPack* PN2S_Solver::FindModelPack(hscID_t id){
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
void PN2S_Solver::Process(PN2S_ModelPack* data){
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


