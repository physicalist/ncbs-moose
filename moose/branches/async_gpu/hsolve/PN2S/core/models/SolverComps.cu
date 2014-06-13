///////////////////////////////////////////////////////////
//  SolverComps.cpp
//  Implementation of the Class SolverComps
//  Created on:      27-Dec-2013 7:57:50 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "SolverComps.h"
#include "solve.h"

#include <assert.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/transform.h>
#include <thrust/functional.h>
#include <thrust/fill.h>

using namespace pn2s::models;
//CuBLAS variables
cublasHandle_t _handle;

cudaStream_t _stream;

__global__ void update_rhs(TYPE_* rhs, TYPE_* vm, TYPE_* cm, TYPE_* rm, size_t size, TYPE_ dt);

SolverComps::SolverComps(): _models(0), _stream(0)
{
}

SolverComps::~SolverComps()
{
}


Error_PN2S SolverComps::AllocateMemory(models::Model * m, models::ModelStatistic& s, cudaStream_t stream)
{
	_stat = s;
	_models = m;
	_stream = stream;

	if(_stat.nCompts == 0)
		return Error_PN2S::NO_ERROR;

	size_t modelSize = s.nCompts*s.nCompts;
	size_t vectorSize = s.nModels * s.nCompts;

	_hm.AllocateMemory(modelSize*s.nModels);
	_rhs.AllocateMemory(vectorSize);
	_Vm.AllocateMemory(vectorSize);
	_Cm.AllocateMemory(vectorSize);
	_Em.AllocateMemory(vectorSize);
	_Rm.AllocateMemory(vectorSize);
	_Ra.AllocateMemory(vectorSize);

	return Error_PN2S::NO_ERROR;
}
Error_PN2S SolverComps::PrepareSolver()
{
//	cudaError_t success;
	cublasStatus_t stat;
	if(_stat.nCompts == 0)
		return Error_PN2S::NO_ERROR;

	//TODO: OpenMP
	//making Hines Matrices
	for(int i=0; i< _stat.nModels;i++ )
	{
		makeHinesMatrix(&_models[i], &_hm[i*_stat.nCompts*_stat.nCompts]);
//		_printMatrix_Column(nComp,nComp, &_hm[i*modelSize]);
	}

	//Copy to GPU
	_hm.Host2Device_Async(_stream);
	_Em.Host2Device_Async(_stream);

//	//Create Cublas
//	if ( cublasCreate(&_handle) != CUBLAS_STATUS_SUCCESS)
//	{
//		return Error_PN2S(Error_PN2S::CuBLASError,
//				"CUBLAS initialization failed");
//	}

	cudaDeviceSynchronize();
	return Error_PN2S::NO_ERROR;
}

void SolverComps::Input()
{
	//Copy to GPU
	_rhs.Send2Device_Async(_Em,_stream); // Em -> rhs
	_Rm.Host2Device_Async(_stream);
	_Vm.Host2Device_Async(_stream);
	_Cm.Host2Device_Async(_stream);
}

void SolverComps::Process()
{
	UpdateMatrix();

	int ret = dsolve_batch (_hm.device, _rhs.device, _Vm.device, _stat.nCompts, _stat.nModels, _stream);
	assert(!ret);
}


void SolverComps::Output()
{
	_Vm.Device2Host_Async(_stream);
}

/**
 * 			UPDATE MATRIX
 *
 * RHS = Vm * Cm / ( dt / 2.0 ) + Em/Rm;
 *
 */
//#define USE_THRUST
#ifdef USE_THRUST
template< class ValueType >
struct update_rhs_functor
{
	typedef ValueType value_type;

	const double dt;
	update_rhs_functor(double __dt) : dt(__dt) {}

	template< class Tuple >
	__host__ __device__
	void operator() ( Tuple t )
	{
		thrust::get<0>(t) = thrust::get<1>(t) *
				thrust::get<2>(t) / dt * 2.0 +
				thrust::get<0>(t) / thrust::get<3>(t);
	}
};
#else

__global__ void update_rhs(TYPE_* rhs, TYPE_* vm, TYPE_* cm, TYPE_* rm, size_t size, TYPE_ dt)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    rhs[idx] = vm[idx] * cm[idx] / dt * 2.0 + rhs[idx] / rm[idx];
}

#endif

void SolverComps::UpdateMatrix()
{
	uint vectorSize = _stat.nModels * _stat.nCompts;

#ifndef USE_THRUST
	dim3 threads, blocks;
	threads=dim3(512, 1);
	blocks=dim3(vectorSize / threads.x, 1);


	update_rhs <<<blocks, threads,0, _stream>>> (
			_rhs.device,
			_Vm.device,
			_Cm.device,
			_Rm.device,
			vectorSize,
			_stat.dt);

#else
	thrust::for_each(
		thrust::make_zip_iterator(
				thrust::make_tuple(
						_rhs.DeviceStart(),
						_Vm.DeviceStart(),
						_Cm.DeviceStart(),
						_Rm.DeviceStart())),

		thrust::make_zip_iterator(
				thrust::make_tuple(
						_rhs.DeviceEnd(),
						_Vm.DeviceEnd(),
						_Cm.DeviceEnd(),
						_Rm.DeviceEnd())),

		update_rhs_functor< TYPE_ >( _stat.dt ) );
#endif
//	getVector(vectorSize, _rhs,_rhs_dev); //TODO maybe is not necessary

}


void SolverComps::makeHinesMatrix(models::Model *model, TYPE_ * matrix)
{
	/*
	 * Some convenience variables
	 */
	vector< double > CmByDt(_stat.nCompts);
	vector< double > Ga(_stat.nCompts);
	for ( unsigned int i = 0; i < _stat.nCompts; i++ ) {
		TYPE_ cm = GetValue(model->compts[ i ].location.index, FIELD::CM); //TODO: check values are equal _Cm[i] or not?
		TYPE_ ra = GetValue(model->compts[ i ].location.index, FIELD::RA);

		CmByDt[i] = cm / ( _stat.dt / 2.0 ) ;
		Ga[i] =  2.0 / ra ;
	}

	/* Each entry in 'coupled' is a list of electrically coupled compartments.
	 * These compartments could be linked at junctions, or even in linear segments
	 * of the cell.
	 */
	vector< vector< unsigned int > > coupled;
	for ( unsigned int i = 0; i < _stat.nCompts; i++ )
		if ( model->compts[ i ].children.size() >= 1 ) {
			coupled.push_back( model->compts[ i ].children );
			coupled.back().push_back( i );
		}

	// Setting diagonal elements
	for ( unsigned int i = 0; i < _stat.nCompts; i++ )
	{
		TYPE_ rm = GetValue(model->compts[ i ].location.index, FIELD::RM);
		matrix[ i * _stat.nCompts + i ] = (TYPE_)(CmByDt[ i ] + 1.0 / rm);
	}


	double gi;
	vector< vector< unsigned int > >::iterator group;
	vector< unsigned int >::iterator ic;
	for ( group = coupled.begin(); group != coupled.end(); ++group ) {
		double gsum = 0.0;

		for ( ic = group->begin(); ic != group->end(); ++ic )
			gsum += Ga[ *ic ];

		for ( ic = group->begin(); ic != group->end(); ++ic ) {
			gi = Ga[ *ic ];

			matrix[ *ic * _stat.nCompts + *ic ] += (TYPE_) (gi * ( 1.0 - gi / gsum ));
		}
	}


	// Setting off-diagonal elements
	double gij;
	vector< unsigned int >::iterator jc;
	for ( group = coupled.begin(); group != coupled.end(); ++group ) {
		double gsum = 0.0;

		for ( ic = group->begin(); ic != group->end(); ++ic )
			gsum += Ga[ *ic ];

		for ( ic = group->begin(); ic != group->end() - 1; ++ic ) {
			for ( jc = ic + 1; jc != group->end(); ++jc ) {
				gij = Ga[ *ic ] * Ga[ *jc ] / gsum;

				matrix[ *ic * _stat.nCompts + *jc ] = (TYPE_)(-gij);
				matrix[ *jc * _stat.nCompts + *ic ] = (TYPE_)(-gij);
			}
		}
	}
}

void SolverComps::SetValue(int index, FIELD::TYPE field, TYPE_ value)
{
	switch(field)
	{
		case FIELD::CM:
			_Cm[index] = value;
			break;
		case FIELD::EM:
			_Em[index] = value;
			break;
		case FIELD::RM:
			_Rm[index] = value;
			break;
		case FIELD::RA:
			_Ra[index] = value;
			break;
		case FIELD::VM:
			_Vm[index] = value;
			break;
		case FIELD::INIT_VM:
			_Vm[index] = value;
			break;
	}
}

TYPE_ SolverComps::GetValue(int index, FIELD::TYPE field)
{
	switch(field)
	{
		case FIELD::CM:
			return _Cm[index];
		case FIELD::EM:
			return _Em[index];
		case FIELD::RM:
			return _Rm[index];
		case FIELD::RA:
			return _Ra[index];
		case FIELD::VM:
			return _Vm[index];
		case FIELD::INIT_VM:
			return _Vm[index];
	}
}

//TYPE_ (*SolverComps::Fetch_Func) (uint, SolverComps::Fields);
