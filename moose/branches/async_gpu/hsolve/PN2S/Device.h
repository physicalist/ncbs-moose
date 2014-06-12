///////////////////////////////////////////////////////////
//  Device.h
//  Implementation of the Class Device
//  Created on:      26-Dec-2013 4:18:01 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#pragma once

#include "headers.h"
#include "core/Solver.h"
#include <omp.h>

namespace pn2s
{

class Device
{
public:
	Error_PN2S SelectDevice();

	int id;
	double _dt;
	vector<ModelPack> _modelPacks;

	Device(int _id);
	virtual ~Device();

	Error_PN2S GenerateModelPacks(double dt,
			models::Model *m,
			size_t start, size_t end,
			int32_t address); //Device address
	void PrepareSolvers();

	void Destroy();
	void Process();

private:
//	int _queue_size;
//	void task1_prepareInput(omp_lock_t& _empty_lock,omp_lock_t& _full_lock, int& state);
//	void task2_doProcess(omp_lock_t& _empty_lock_input,	omp_lock_t& _full_lock_input,omp_lock_t& _empty_lock_output, int& state);
//	void task3_prepareOutput(omp_lock_t& _empty_lock, int& state);

};
}

