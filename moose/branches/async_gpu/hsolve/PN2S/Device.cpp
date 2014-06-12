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

//Create streams
cudaStream_t* streams;
int nstreams = DEFAULT_STREAM_NUMBER;

Device::Device(int _id): id(_id), _dt(1){
	_modelPacks.clear();

	/**
	 * Check configuration
	 */

	nstreams = 6;
	/**
	 * Device initialization
	 */
	streams = (cudaStream_t *) malloc(nstreams * sizeof(cudaStream_t));
	//Create Streams
	for (int i = 0; i < nstreams; ++i) {
		cudaStreamCreate(&streams[i]);
	}
}

Device::~Device(){
//	if(_modelPacks.size())
//		cudaDeviceReset();
}

void Device::Destroy(){
//	for (int i = 0; i < nstreams; i++)
//	{
//		cudaStreamDestroy(streams[i]);
//	}
//	cudaDeviceReset();
}

Error_PN2S Device::GenerateModelPacks(double dt, models::Model *m, size_t start, size_t end, int32_t address){
	_dt = dt;

	//Distribute model into packs
	models::Model *m_start = &m[start];
	size_t nModel = end - start;

	if(nModel <= 0)
		return Error_PN2S::EMPTY;

	//TODO: Minimum size for models and change strategy for distribution of models

	//If number of models are less than streams, reduce stream number
	if (nstreams > nModel)
	{
		for (int i = nModel; i < nstreams; i++)
		{
			cudaStreamDestroy(streams[i]);
		}
		nstreams = nModel;
	}

	size_t nModel_in_pack = nModel/nstreams;
	_modelPacks.resize(nstreams);
	for (int pack = 0; pack < nstreams; ++pack) {
		//Check nComp for each compartments and update it's fields
		if(pack == nstreams-1) //Last one carries extra parts
		{
			 nModel_in_pack += nModel%nstreams;
		}
		size_t nCompt = m_start->compts.size();
		for (int i = 0; i < nModel_in_pack; ++i) {
			assert(m_start[i].compts.size() == nCompt);
			for (int c = 0; c < nCompt; ++c) {
				//Assign address for each compartment
				m_start[i].compts[c].location.address = pack;
				//Assign index of each object in a modelPack
				m_start[i].compts[c].location.index = i;
			}
		}

		// Allocate memory for Modelpacks
		ModelStatistic stat(dt, nModel_in_pack, nCompt);
		_modelPacks[pack].Allocate(m_start, stat,streams[pack%nstreams]);
		m_start += nModel_in_pack;
	}
	return Error_PN2S::NO_ERROR;
}

void Device::PrepareSolvers(){
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

#ifdef USE_TBB
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
#else

	for (vector<ModelPack >::iterator it = _modelPacks.begin(); it != _modelPacks.end(); ++it)
	{
		it->Input();
	}
	for (vector<ModelPack >::iterator it = _modelPacks.begin(); it != _modelPacks.end(); ++it)
	{
		it->Process();
	}
	for (vector<ModelPack >::iterator it = _modelPacks.begin(); it != _modelPacks.end(); ++it)
	{
		it->Output();
	}
	cudaDeviceSynchronize();
#endif
}
