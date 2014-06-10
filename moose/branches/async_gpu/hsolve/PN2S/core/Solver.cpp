///////////////////////////////////////////////////////////
//  Solver.cpp
//  Implementation of the Class Solver
//  Created on:      26-Dec-2013 4:21:51 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "Solver.h"
#include <assert.h>

using namespace pn2s;

Solver::Solver()
{
	_dt = 1; //1ms
	modelPacks.clear();
	_modelToPackMap.clear();
}

Solver::~Solver()
{

}

Error_PN2S Solver::PrepareSolver(vector<models::Model > &m,  double dt){
	_dt = dt;

	//TODO: Generate model packs
	modelPacks.resize(1);
//	modelPacks[0].SetDt(_dt);

	//Prepare solver for each modelpack
//	ErrorType_PN2S res = modelPacks[0].PrepareSolver(m);

	//Assign keys to modelPack, to be able to find later
	for(vector<models >::iterator it = m.begin(); it != m.end(); ++it) {
		_modelToPackMap[it->id] = &modelPacks[0];
	}
	return res;
}

ModelPack* Solver::FindModelPack(hscID_t id){
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
void Solver::Process(ModelPack* data){
	ErrorType_PN2S res = data->Process();
	assert(!res);
}


#ifdef DO_UNIT_TESTS
#include <cassert>

using namespace std;

void testSolver()
{
	cout << "testSolver" << endl <<flush;
}
#endif

template class Solver<double, ARCH_SM30>;
