///////////////////////////////////////////////////////////
//  PN2S_Solver.cpp
//  Implementation of the Class PN2S_Solver
//  Created on:      26-Dec-2013 4:21:51 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_Solver.h"
#include <assert.h>

template <typename T, int arch>
PN2S_Solver<T,arch>::PN2S_Solver()
{
	_dt = 1; //1ms
	modelPacks.clear();
	_modelToPackMap.clear();
}

template <typename T, int arch>
PN2S_Solver<T,arch>::~PN2S_Solver()
{

}

template <typename T, int arch>
Error_PN2S PN2S_Solver<T,arch>::PrepareSolver(vector<PN2SModel<T,arch> > &m,  double dt){
	_dt = dt;

	//TODO: Generate model packs
	modelPacks.resize(1);
	modelPacks[0].SetDt(_dt);

	//Prepare solver for each modelpack
	ErrorType_PN2S res = modelPacks[0].PrepareSolver(m);

	//Assign keys to modelPack, to be able to find later
	for(vector<PN2SModel<T,arch> >::iterator it = m.begin(); it != m.end(); ++it) {
		_modelToPackMap[it->id] = &modelPacks[0];
	}
	return res;
}

template <typename T, int arch>
PN2S_ModelPack<T,arch>* PN2S_Solver<T,arch>::FindModelPack(hscID_t id){
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
void PN2S_Solver<T,arch>::Process(PN2S_ModelPack<T,arch>* data){
	ErrorType_PN2S res = data->Process();
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

template class PN2S_Solver<double, ARCH_SM30>;
