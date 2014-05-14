///////////////////////////////////////////////////////////
//  PN2S_Device.h
//  Implementation of the Class PN2S_Device
//  Created on:      26-Dec-2013 4:18:01 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#if !defined(AB1F38D16_FE78_4893_BC39_356BCBBCF345__INCLUDED_)
#define AB1F38D16_FE78_4893_BC39_356BCBBCF345__INCLUDED_
#include "Definitions.h"
#include "core/PN2S_Solver.h"
#include <omp.h>

class PN2S_Device
{
public:
	hscError SelectDevice();

	int id;
	double _dt;
	vector<PN2S_ModelPack<CURRENT_TYPE,CURRENT_ARCH> > _modelPacks;

	PN2S_Device(int _id);
	virtual ~PN2S_Device();

	hscError PrepareSolver(vector<PN2SModel<CURRENT_TYPE,CURRENT_ARCH> > &m,  double dt);
	void Process();
	hscError Setup();


private:
	int _queue_size;
	void task1_prepareInput(omp_lock_t& _empty_lock,omp_lock_t& _full_lock, int& state);
	void task2_doProcess(omp_lock_t& _empty_lock_input,	omp_lock_t& _full_lock_input,omp_lock_t& _empty_lock_output, int& state);
	void task3_prepareOutput(omp_lock_t& _empty_lock, int& state);

};


#endif // !defined(AB1F38D16_FE78_4893_BC39_356BCBBCF345__INCLUDED_)
