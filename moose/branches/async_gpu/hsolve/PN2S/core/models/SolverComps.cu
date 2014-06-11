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

//Thrust Variables
//thrust::device_vector<int> d_rhs = h_vec;

SolverComps::SolverComps()
{
//	_hm.fieldType = Field::TYPE_IO;
}

SolverComps::~SolverComps()
{
//	if (_hm) free(_hm);
//	if(_rhs) free(_rhs);
//	if (_Vm) free(_Vm);
//	if (_Cm) free(_Cm);
//	if (_Em) free(_Em);
//	if (_Rm) free(_Rm);
//	if (_hm_dev) cudaFree(_hm_dev);
//	if(_rhs_dev) cudaFree(_rhs_dev);
//	if (_Vm_dev) cudaFree(_Vm_dev);
//	if (_Cm_dev) cudaFree(_Cm_dev);
//	if (_Em_dev) cudaFree(_Em_dev);
//	if (_Rm_dev) cudaFree(_Rm_dev);
}


Error_PN2S SolverComps::AllocateMemory(models::Model * m, models::ModelStatistic& s)
{
	_stat = s;
	_models = m;

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
	_hm.Host2Device_Sync();
	_Em.Host2Device_Sync();

	//Create Cublas
	if ( cublasCreate(&_handle) != CUBLAS_STATUS_SUCCESS)
	{
		return Error_PN2S(Error_PN2S::CuBLASError,
				"CUBLAS initialization failed");
	}

	cudaDeviceSynchronize();
	return Error_PN2S::NO_ERROR;
}

Error_PN2S SolverComps::Input()
{
	//Copy to GPU
	_rhs.Send2Device(_Em); // Em -> rhs
	_Rm.Host2Device_Sync();
	_Vm.Host2Device_Sync();
	_Cm.Host2Device_Sync();

	return Error_PN2S::NO_ERROR;
}

Error_PN2S SolverComps::Process()
{
	UpdateMatrix();

    //Solve
	int ret = dsolve_batch (_hm.device, _rhs.device, _Vm.device, _stat.nCompts, _stat.nModels);
    assert(!ret);
	return Error_PN2S::NO_ERROR;
    //_printVector(nModel*nComp, _Vm);
}


Error_PN2S SolverComps::Output()
{
	_Vm.Device2Host_Sync();

//	for(int i=0; i< _ids.size();i++ )
//	{
//		uint gid = _ids[i];
////		SetValue_Func(gid,VM_FIELD, _Vm[i]);
////		SetValue_Func(gid,VM_FIELD, 10);
//	}

	return Error_PN2S::NO_ERROR;
	//_printVector(nModel*nComp, _Vm);
}

/**
 * 			UPDATE MATRIX
 *
 * RHS = Vm * Cm / ( dt / 2.0 ) + Em/Rm;
 *
 */

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


Error_PN2S SolverComps::UpdateMatrix()
{
	uint vectorSize = _stat.nModels * _stat.nCompts;

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

//	getVector(vectorSize, _rhs,_rhs_dev); //TODO maybe is not necessary

	return Error_PN2S::NO_ERROR;
}


void SolverComps::makeHinesMatrix(models::Model *model, TYPE_ * matrix)
{
	/*
	 * Some convenience variables
	 */
//	vector< double > CmByDt(_stat.nCompts);
//	vector< double > Ga(_stat.nCompts);
//	for ( unsigned int i = 0; i < _stat.nCompts; i++ ) {
//		TYPE_ cm = Fetch_Func(model->compts[ i ].gid,CM_FIELD);
//		TYPE_ ra = Fetch_Func(model->compts[ i ].gid,RA_FIELD);
//
//		CmByDt[i] = cm / ( _stat.dt / 2.0 ) ;
//		Ga[i] =  2.0 / ra ;
//	}
//
//	/* Each entry in 'coupled' is a list of electrically coupled compartments.
//	 * These compartments could be linked at junctions, or even in linear segments
//	 * of the cell.
//	 */
//	vector< vector< unsigned int > > coupled;
//	for ( unsigned int i = 0; i < _stat.nCompts; i++ )
//		if ( model->compts[ i ].children.size() >= 1 ) {
//			coupled.push_back( model->compts[ i ].children );
//			coupled.back().push_back( i );
//		}
//
//	// Setting diagonal elements
//	for ( unsigned int i = 0; i < _stat.nCompts; i++ )
//	{
//		TYPE_ rm = Fetch_Func(model->compts[ i ].gid,RM_FIELD);
//		matrix[ i * _stat.nCompts + i ] = (TYPE_)(CmByDt[ i ] + 1.0 / rm);
//	}
//
//
//	double gi;
//	vector< vector< unsigned int > >::iterator group;
//	vector< unsigned int >::iterator ic;
//	for ( group = coupled.begin(); group != coupled.end(); ++group ) {
//		double gsum = 0.0;
//
//		for ( ic = group->begin(); ic != group->end(); ++ic )
//			gsum += Ga[ *ic ];
//
//		for ( ic = group->begin(); ic != group->end(); ++ic ) {
//			gi = Ga[ *ic ];
//
//			matrix[ *ic * _stat.nCompts + *ic ] += (TYPE_) (gi * ( 1.0 - gi / gsum ));
//		}
//	}
//
//
//	// Setting off-diagonal elements
//	double gij;
//	vector< unsigned int >::iterator jc;
//	for ( group = coupled.begin(); group != coupled.end(); ++group ) {
//		double gsum = 0.0;
//
//		for ( ic = group->begin(); ic != group->end(); ++ic )
//			gsum += Ga[ *ic ];
//
//		for ( ic = group->begin(); ic != group->end() - 1; ++ic ) {
//			for ( jc = ic + 1; jc != group->end(); ++jc ) {
//				gij = Ga[ *ic ] * Ga[ *jc ] / gsum;
//
//				matrix[ *ic * _stat.nCompts + *jc ] = (TYPE_)(-gij);
//				matrix[ *jc * _stat.nCompts + *ic ] = (TYPE_)(-gij);
//			}
//		}
//	}
}

void SolverComps::SetValue(Compartment* c, FIELD::TYPE field, TYPE_ value)
{
	switch(field)
	{
		case FIELD::CM_FIELD:
			_Cm[c->_index] = value;
			break;
		case FIELD::EM_FIELD:
			_Em[c->_index] = value;
			break;
		case FIELD::RM_FIELD:
			_Rm[c->_index] = value;
			break;
		case FIELD::RA_FIELD:
			_Ra[c->_index] = value;
			break;
		case FIELD::VM_FIELD:
			_Vm[c->_index] = value;
			break;
		case FIELD::INIT_VM_FIELD:
			_Vm[c->_index] = value;
			break;
	}
}

TYPE_ SolverComps::GetValue(Compartment* c, FIELD::TYPE field)
{
	switch(field)
	{
		case FIELD::CM_FIELD:
			return _Cm[c->_index];
		case FIELD::EM_FIELD:
			return _Em[c->_index];
		case FIELD::RM_FIELD:
			return _Rm[c->_index];
		case FIELD::RA_FIELD:
			return _Ra[c->_index];
		case FIELD::VM_FIELD:
			return _Vm[c->_index];
		case FIELD::INIT_VM_FIELD:
			return _Vm[c->_index];
	}
}

//TYPE_ (*SolverComps::Fetch_Func) (uint, SolverComps::Fields);
