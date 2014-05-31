///////////////////////////////////////////////////////////
//  Solver.cpp
//  Implementation of the Class Solver
//  Created on:      26-Dec-2013 4:21:51 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "Solver.h"
#include <assert.h>

using namespace pn2s;

template <typename T, int arch>
Solver<T,arch>::Solver()
{
	_dt = 1; //1ms
	modelPacks.clear();
	_modelToPackMap.clear();
}

template <typename T, int arch>
Solver<T,arch>::~Solver()
{

}

template <typename T, int arch>
Error_PN2S Solver<T,arch>::PrepareSolver(vector<models::Model<T> > &m,  double dt){
	_dt = dt;

	//TODO: Generate model packs
	modelPacks.resize(1);
	modelPacks[0].SetDt(_dt);

	//Prepare solver for each modelpack
	ErrorType_PN2S res = modelPacks[0].PrepareSolver(m);

	//Assign keys to modelPack, to be able to find later
	for(vector<models<T,arch> >::iterator it = m.begin(); it != m.end(); ++it) {
		_modelToPackMap[it->id] = &modelPacks[0];
	}
	return res;
}

template <typename T, int arch>
ModelPack<T,arch>* Solver<T,arch>::FindModelPack(hscID_t id){
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
template <typename T, int arch>
void Solver<T,arch>::Process(ModelPack<T,arch>* data){
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
