///////////////////////////////////////////////////////////
//  PN2S_Device.cpp
//  Implementation of the Class PN2S_Device
//  Created on:      26-Dec-2013 4:18:01 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#include "PN2S_Device.h"
#include "PN2S.h"
#include <cuda.h>
#include <cuda_runtime.h>

PN2S_Device::PN2S_Device(int _id): id(_id), _dt(1), _queue_size(1){
	cudaDeviceReset();
	_modelPacks.clear();
}

PN2S_Device::~PN2S_Device(){
	cudaDeviceReset();
}

Error_PN2S PN2S_Device::Reinit(vector<PN2SModel<CURRENT_TYPE,CURRENT_ARCH> > &m,  double dt){
	_dt = dt;

	//TODO: Generate model packs
	_modelPacks.resize(1);
	_modelPacks[0].SetDt(_dt);

	//Prepare solver for each modelpack
	Error_PN2S res = _modelPacks[0].Reinit(m);

//	//Assign keys to modelPack, to be able to find later
//	for(vector<PN2SModel>::iterator it = m.begin(); it != m.end(); ++it) {
//		_modelToPackMap[it->id] = &modelPacks[0];
//	}
	return res;
}

/**
 * Multithread tasks. Two queues, each one follow producer-consumer model
 */

int _iq_size;
int _iq_limit;
std::deque<PN2S_ModelPack<CURRENT_TYPE,CURRENT_ARCH> > _iq;

//No limit size for output queue
int _oq_size;
std::deque<PN2S_ModelPack<CURRENT_TYPE,CURRENT_ARCH> > _oq;

void PN2S_Device::Process()
{
	if(_modelPacks.size() < 1)
		return;

	//State variable for three threads
	int state=0;	//Start:0, task1 done: 1, task2 done: 2, Stop: 3

	//Lock initialization
	omp_lock_t _empty_lock_input;
	omp_lock_t _full_lock_input;
	omp_lock_t _empty_lock_output;
	omp_init_lock(&_empty_lock_input);
	omp_init_lock(&_full_lock_input);
	omp_init_lock(&_empty_lock_output);
	omp_set_lock(&_empty_lock_input);
	omp_set_lock(&_full_lock_input);
	omp_set_lock(&_empty_lock_output);

	//TODO: Replace omp with pthread
	_iq_limit = _queue_size;
	#pragma omp parallel \
		shared(_iq, _iq_size, state) \
		firstprivate(_iq_limit) \
		num_threads(3)
	{
		int tid = omp_get_thread_num();
		if(tid%3 == 0)
		{
			task1_prepareInput(_empty_lock_input,_full_lock_input, state);
		}
		else if(tid%3==1)
		{
			task2_doProcess(_empty_lock_input,_full_lock_input,_empty_lock_output,  state);
		}
		else if(tid%3==2)
		{
			task3_prepareOutput(_empty_lock_output,  state);
		}
	}

	omp_destroy_lock(&_empty_lock_input);

}

void PN2S_Device::task1_prepareInput(omp_lock_t& _empty_lock,omp_lock_t& _full_lock, int& state) {
	for (vector<PN2S_ModelPack<CURRENT_TYPE,CURRENT_ARCH> >::iterator it = _modelPacks.begin(); it != _modelPacks.end(); ++it)
	{
		if(_iq_size >= _iq_limit)
			omp_set_lock(&_full_lock);

		//Prepare Input
		it->Input();

		//Add to the ready task list
		_iq.push_back(*it);

		#pragma omp critical
		{
			_iq_size++;
			#pragma omp flush(_iq_size)
			cout<<"Thread = "<< omp_get_thread_num()<<"Limit "<<_iq_limit<<" Add"<<"  QSize="<<_iq.size()<<"size="<<_iq_size<<endl<<flush;
		}
		omp_unset_lock(&_empty_lock);
	}
	state = 1;
	#pragma omp flush(state)
}

void PN2S_Device::task2_doProcess(omp_lock_t& _empty_lock_input,
		omp_lock_t& _full_lock_input,
		omp_lock_t& _empty_lock_output,
		int& state)
{
	do {
		if(_iq.size() == 0)
			omp_set_lock(&_empty_lock_input);

		PN2S_ModelPack<CURRENT_TYPE,CURRENT_ARCH>& t= _iq[0];
		_iq.pop_front();

		//Do Process
		t.Process();

		//Add task to output
		_oq.push_back(t);

		#pragma omp critical
		{
			_iq_size--;
			_oq_size++;
			cout<<"Thread = "<< omp_get_thread_num()<<"Limit "<<_iq_limit<<" Process "<<"  QSize="<<_iq.size() <<"size="<<_iq_size<<endl<<flush;
		}

		omp_unset_lock(&_full_lock_input);
		omp_unset_lock(&_empty_lock_output);

	} while (state < 1);

	state = 2;
	#pragma omp flush(state)
}

void PN2S_Device::task3_prepareOutput(omp_lock_t& _empty_lock, int& state) {
	do {
		if(_oq.size() == 0)
			omp_set_lock(&_empty_lock);

		sleep(1);
		PN2S_ModelPack<CURRENT_TYPE,CURRENT_ARCH>* t= &_oq[0];
		_oq.pop_front();

		//Output routins
		t->Output();

		#pragma omp critical
		{
			_oq_size--;
			cout<<"Thread = "<< omp_get_thread_num()<<" Output "<<"  QSize="<<_oq.size() <<"size="<<_oq_size<<endl<<flush;
		}
	} while(state < 2);

	state = 3;
	#pragma omp flush(state)
}

