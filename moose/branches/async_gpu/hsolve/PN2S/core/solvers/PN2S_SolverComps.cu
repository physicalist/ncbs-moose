///////////////////////////////////////////////////////////
//  PN2S_SolverComps.cpp
//  Implementation of the Class PN2S_SolverComps
//  Created on:      27-Dec-2013 7:57:50 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_SolverComps.h"
#include "solve.h"

#include <assert.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/transform.h>
#include <thrust/functional.h>
#include <thrust/fill.h>

//CuBLAS variables
cublasHandle_t _handle;

//Thrust Variables
//thrust::device_vector<int> d_rhs = h_vec;


template <typename T, int arch>
PN2S_SolverComps<T,arch>::PN2S_SolverComps()
{
//	_hm.fieldType = PN2S_Field<T,arch>::TYPE_IO;
}

template <typename T, int arch>
PN2S_SolverComps<T,arch>::~PN2S_SolverComps()
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

template <typename T, int arch>
Error_PN2S PN2S_SolverComps<T,arch>::PrepareSolver(vector<PN2SModel<T,arch> > &network, PN2S_NetworkAnalyzer<T,arch> &analyzer)
{
//	cudaError_t success;
	cublasStatus_t stat;

	nModel = analyzer.nModel;
	nComp = analyzer.nComp;
	uint modelSize = nComp*nComp;
	uint vectorSize = nModel * nComp;

	_hm.AllocateMemory(modelSize*nModel);
	_rhs.AllocateMemory(vectorSize);
	_Vm.AllocateMemory(vectorSize);
	_Cm.AllocateMemory(vectorSize);
	_Em.AllocateMemory(vectorSize);
	_Rm.AllocateMemory(vectorSize);


	//TODO: OpenMP
	int idx = 0;
	for(int i=0; i< nModel;i++ )
	{
		for(int n=0; n< nComp;n++)
		{
			//Initialize values
			uint gid = analyzer.allCompartments[idx]->gid;
			_Vm[idx] = GetValue_Func(gid,INIT_VM_FIELD);
			_Cm[idx] = GetValue_Func(gid,CM_FIELD);
			_Em[idx] = GetValue_Func(gid,EM_FIELD);
			_Rm[idx] = GetValue_Func(gid,RM_FIELD);
			idx++;
		}
		//making Hines Matrices
		makeHinesMatrix(&network[i], &_hm[i*modelSize]);
//		_printMatrix_Column(nComp,nComp, &_hm[i*modelSize]);
	}

	//Copy to GPU
	_hm.Host2Device_Sync();
	_rhs.Host2Device_Sync();
	_Vm.Host2Device_Sync();
	_Cm.Host2Device_Sync();
	_Em.Host2Device_Sync();
	_Rm.Host2Device_Sync();

	//Create Cublas
	if ( cublasCreate(&_handle) != CUBLAS_STATUS_SUCCESS)
	{
		return Error_PN2S(Error_PN2S::CuBLASError,
				"CUBLAS initialization failed");
	}

	cudaDeviceSynchronize();
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S PN2S_SolverComps<T,arch>::Process()
{
	UpdateMatrix();

    //Solve
	int ret = dsolve_batch (_hm.device, _rhs.device, _Vm.device, nComp, nModel);
    assert(!ret);

    _Vm.Device2Host_Sync();

    //_printVector(nModel*nComp, _Vm);

	return Error_PN2S::NO_ERROR;
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

template <typename T, int arch>
Error_PN2S PN2S_SolverComps<T,arch>::UpdateMatrix()
{
	uint vectorSize = nModel * nComp;

	//Copy to GPU
	_rhs.Send2Device(_Em); // Em -> rhs
	_Rm.Host2Device_Sync();
	_Vm.Host2Device_Sync();
	_Cm.Host2Device_Sync();

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

		update_rhs_functor< T >( _dt ) );

//	getVector(vectorSize, _rhs,_rhs_dev); //TODO maybe is not necessary

	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
void PN2S_SolverComps<T,arch>::makeHinesMatrix(PN2SModel<T,arch> *model, T * matrix)
{
	/*
	 * Some convenience variables
	 */
	vector< double > CmByDt(nComp);
	vector< double > Ga(nComp);
	for ( unsigned int i = 0; i < nComp; i++ ) {
		T cm = GetValue_Func(model->compts[ i ].gid,CM_FIELD);
		T ra = GetValue_Func(model->compts[ i ].gid,RA_FIELD);

		CmByDt[i] = cm / ( _dt / 2.0 ) ;
		Ga[i] =  2.0 / ra ;
	}

	/* Each entry in 'coupled' is a list of electrically coupled compartments.
	 * These compartments could be linked at junctions, or even in linear segments
	 * of the cell.
	 */
	vector< vector< unsigned int > > coupled;
	for ( unsigned int i = 0; i < nComp; i++ )
		if ( model->compts[ i ].children.size() >= 1 ) {
			coupled.push_back( model->compts[ i ].children );
			coupled.back().push_back( i );
		}

	// Setting diagonal elements
	for ( unsigned int i = 0; i < nComp; i++ )
	{
		T rm = GetValue_Func(model->compts[ i ].gid,RM_FIELD);
		matrix[ i * nComp + i ] = (T)(CmByDt[ i ] + 1.0 / rm);
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

			matrix[ *ic * nComp + *ic ] += (T) (gi * ( 1.0 - gi / gsum ));
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

				matrix[ *ic * nComp + *jc ] = (T)(-gij);
				matrix[ *jc * nComp + *ic ] = (T)(-gij);
			}
		}
	}
}

template <typename T, int arch>
T (*PN2S_SolverComps<T,arch>::GetValue_Func) (uint id, PN2S_SolverComps<T,arch>::Fields field);


template class PN2S_SolverComps<double, ARCH_SM30>;
