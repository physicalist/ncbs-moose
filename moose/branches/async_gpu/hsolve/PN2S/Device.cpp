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

Error_PN2S Device::Reinit(vector<models::Model> &m,  double dt){
	_dt = dt;

	//TODO: Generate model packs
	_modelPacks.resize(1);
	_modelPacks[0].SetDt(_dt);

	//Prepare solver for each modelpack
	Error_PN2S res = _modelPacks[0].Reinit(m);

//	//Assign keys to modelPack, to be able to find later
//	for(vector<models>::iterator it = m.begin(); it != m.end(); ++it) {
//		_modelToPackMap[it->id] = &modelPacks[0];
//	}
	return res;
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
