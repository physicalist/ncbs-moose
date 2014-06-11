///////////////////////////////////////////////////////////
//  Device.cpp
//  Implementation of the Class Device
//  Created on:      26-Dec-2013 4:18:01 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#include "Device.h"
#include "headers.h"
#include <cuda.h>
#include <cuda_runtime.h>
// tbb related header files
#include "tbb/flow_graph.h"
#include "tbb/atomic.h"
#include "tbb/tick_count.h"
#include "core/models/ModelStatistic.h"
#include <assert.h>

using namespace pn2s;
using namespace std;
using namespace tbb;
using namespace tbb::flow;

using namespace pn2s::models;

Device::Device(int _id): id(_id), _dt(1), _queue_size(1){
	cudaDeviceReset();
	_modelPacks.clear();
}

Device::~Device(){
	cudaDeviceReset();
}

Error_PN2S Device::GenerateModelPacks(double dt, models::Model *m, size_t start, size_t end, int32_t address){
	_dt = dt;
	models::Model *m_start = &m[start];

	size_t nModel = end - start;
	//Check nComp for each compartments
	size_t nCompt = m_start->compts.size();
	for (int i = 0; i < nModel; ++i) {
		assert(m_start[i].compts.size() != nCompt);
		for (int c = 0; c < nCompt; ++c) {
			m_start[i].compts[c].address = address;
		}
	}
	//Check network structure
	ModelStatistic stat(dt, nModel, nCompt);

	//TODO: Generate model packs
	_modelPacks.resize(1);

	//Prepare solver for each modelpack
//	address = address + 0;
	Error_PN2S res = _modelPacks[0].Allocate(m_start, stat);

	//Update Data
//	for(vector<models>::iterator it = m.begin(); it != m.end(); ++it) {
//		_modelToPackMap[it->id] = &modelPacks[0];
//	}
	return res;
}

Error_PN2S Device::PrepareSolvers(){
	for(vector<ModelPack>::iterator it = _modelPacks.begin(); it != _modelPacks.end(); ++it) {
		it->PrepareSolvers();
	}
}


/**
 * Multithread tasks section
 */


//#ifdef PN2S_DEBUG

struct input_body {
	ModelPack* operator()( ModelPack *m ) {
		_D(std::cout<< "Input" << m<<endl<<flush);
		m->Input();
        return m;
    }
};

struct process_body{
	ModelPack* operator()( ModelPack* m) {
		_D(std::cout<< "Process" << m<<endl<<flush);
		m->Process();
        return m;
    }
};

struct output_body{
	ModelPack* operator()( ModelPack* m ) {
		_D(std::cout<< "Output" << m<<endl<<flush);
		m->Output();
        return m;
    }
};

void Device::Process()
{
	uint model_n = _modelPacks.size();
	if(model_n < 1)
		return;

	graph scheduler;

	broadcast_node<ModelPack*> broadcast(scheduler);
	function_node< ModelPack*, ModelPack* > input_node(scheduler, 1, input_body());
	function_node< ModelPack*, ModelPack* > process_node(scheduler, 1, process_body() );
	function_node< ModelPack*, ModelPack* > output_node(scheduler, 1, output_body() );
	queue_node< ModelPack* > iq_node(scheduler);
	queue_node< ModelPack* > oq_node(scheduler);
	make_edge( broadcast, input_node );
	make_edge( input_node, iq_node);
	make_edge( iq_node, process_node);
	make_edge( process_node, oq_node);
	make_edge( oq_node, output_node);


	for (vector<ModelPack >::iterator it = _modelPacks.begin(); it != _modelPacks.end(); ++it)
	{
		broadcast.try_put(&(*it));
	}
	scheduler.wait_for_all();
}
