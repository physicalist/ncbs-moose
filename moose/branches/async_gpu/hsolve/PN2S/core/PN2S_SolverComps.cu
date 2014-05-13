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

// Macro to catch CUDA errors in CUDA runtime calls
#define CUDA_SAFE_CALL(call)                                          \
do {                                                                  \
    cudaError_t err = call;                                           \
    if (cudaSuccess != err) {                                         \
        fprintf (stderr, "Cuda error in file '%s' in line %i : %s.\n",\
                 __FILE__, __LINE__, cudaGetErrorString(err) );       \
        exit(EXIT_FAILURE);                                           \
    }                                                                 \
} while (0)

template <typename T>
__inline__ hscError sendVector(uint size, T* h, T* d)
{
	cublasStatus_t stat = cublasSetVector(size, sizeof(h[0]),h, 1,d,1);
	if (stat != CUBLAS_STATUS_SUCCESS) {
		cerr << "CUBLAS setting Variables\n";
		return CuBLASError;
	}
	return NO_ERROR;
}

template <typename T>
__inline__ hscError getVector(uint size, T* h, T* d)
{
	cublasStatus_t stat = cublasGetVector(size, sizeof(h[0]),d, 1,h,1);
	if (stat != CUBLAS_STATUS_SUCCESS) {
		cerr << "CUBLAS setting Variables\n";
		return CuBLASError;
	}
	return NO_ERROR;
}

//CuBLAS variables
cublasHandle_t _handle;

//Thrust Variables
//thrust::device_vector<int> d_rhs = h_vec;


template <typename T, int arch>
PN2S_SolverComps<T,arch>::PN2S_SolverComps()
{
	_hm = 0;
	_hm_dev = 0;
	_rhs = 0;
	_rhs_dev = 0;
	_Vm = 0;
	_Vm_dev = 0;
	_Cm = 0;
	_Cm_dev = 0;
	_Em = 0;
	_Em_dev = 0;
	_Rm = 0;
	_Rm_dev = 0;
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
hscError PN2S_SolverComps<T,arch>::PrepareSolver(vector<PN2SModel > &network, PN2S_NetworkAnalyzer &analyzer)
{
//	cudaError_t success;
	cublasStatus_t stat;

	nModel = analyzer.nModel;
	nComp = analyzer.nComp;
	uint modelSize = nComp*nComp;
	uint vectorSize = nModel * nComp;

	//Define memory for host
	CUDA_SAFE_CALL(cudaMallocHost((void **) &_hm, modelSize*nModel * sizeof(_hm[0]))); //Define pinned memories
	CUDA_SAFE_CALL(cudaMallocHost((void **) &_rhs, vectorSize * sizeof(_rhs[0]))); //Define pinned memories
	CUDA_SAFE_CALL(cudaMallocHost((void **) &_Vm, vectorSize * sizeof(_Vm[0]))); 	//Define pinned memories
	CUDA_SAFE_CALL(cudaMallocHost((void **) &_Cm, vectorSize * sizeof(_Cm[0]))); 	//Define pinned memories
	CUDA_SAFE_CALL(cudaMallocHost((void **) &_Em, vectorSize * sizeof(_Em[0]))); 	//Define pinned memories
	CUDA_SAFE_CALL(cudaMallocHost((void **) &_Rm, vectorSize * sizeof(_Rm[0]))); 	//Define pinned memories

	//Define memory for device
	CUDA_SAFE_CALL(cudaMalloc((void **) &_hm_dev, modelSize*nModel * sizeof(_hm_dev[0])));
	CUDA_SAFE_CALL(cudaMalloc((void **) &_rhs_dev, vectorSize * sizeof(_rhs_dev[0])));
	CUDA_SAFE_CALL(cudaMalloc((void **) &_Vm_dev, vectorSize * sizeof(_Vm_dev[0])));
	CUDA_SAFE_CALL(cudaMalloc((void **) &_Cm_dev, vectorSize * sizeof(_Cm_dev[0])));
	CUDA_SAFE_CALL(cudaMalloc((void **) &_Em_dev, vectorSize * sizeof(_Em_dev[0])));
	CUDA_SAFE_CALL(cudaMalloc((void **) &_Rm_dev, vectorSize * sizeof(_Rm_dev[0])));

	//Fill Vectors
	for(int i=0; i< vectorSize;i++ )
	{
		_Vm[i] = analyzer.allCompartments[i]->initVm;
		_Cm[i] = analyzer.allCompartments[i]->Cm;
		_Em[i] = analyzer.allCompartments[i]->Em;
		_Rm[i] = analyzer.allCompartments[i]->Rm;
	}

	//making Hines Matrices
	for(int i=0; i< nModel;i++ )
	{
		makeHinesMatrix(&network[i], &_hm[i*modelSize]);
//		_printMatrix_Column(nComp,nComp, &_hm[i*modelSize]);

		//TODO: Copy matrix from outside of this loop completely
		//Copy Matrices
		stat = cublasSetMatrix(nComp, nComp, sizeof(_hm[0]),
						&_hm[i*modelSize], nComp,
						&_hm_dev[i*modelSize], nComp);

		if (stat != CUBLAS_STATUS_SUCCESS) {
				cerr << "CUBLAS setting Variables\n";
				return CuBLASError;
		}
	}

	//Copy to GPU
	sendVector<T>(vectorSize, _Vm,_Vm_dev);
	sendVector<T>(vectorSize, _Em,_Em_dev);
	sendVector<T>(vectorSize, _Cm,_Cm_dev);
	sendVector<T>(vectorSize, _Rm,_Rm_dev);

	//Create Cublas
	if ( cublasCreate(&_handle) != CUBLAS_STATUS_SUCCESS)
	{
		cerr << "CUBLAS initialization failed\n";
		return CuBLASError;
	}

	cudaDeviceSynchronize();
	return NO_ERROR;
}

template <typename T, int arch>
hscError PN2S_SolverComps<T,arch>::Process()
{
	UpdateMatrix();

    //Solve
	int ret = dsolve_batch (_hm_dev, _rhs_dev, _Vm_dev, nComp, nModel);
    assert(!ret);

    getVector(nModel * nComp, _Vm,_Vm_dev);

    //_printVector(nModel*nComp, _Vm);

	return NO_ERROR;
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
hscError PN2S_SolverComps<T,arch>::UpdateMatrix()
{
	uint vectorSize = nModel * nComp;

	//Copy to GPU
	sendVector<T>(vectorSize, _Em,_rhs_dev); // Em -> rhs
	sendVector<T>(vectorSize, _Rm,_Rm_dev);
	sendVector<T>(vectorSize, _Vm,_Vm_dev);
	sendVector<T>(vectorSize, _Cm,_Cm_dev);

	thrust::device_ptr<T> th_rhs_start(_rhs_dev);
	thrust::device_ptr<T> th_rhs_end (_rhs_dev+vectorSize);

	thrust::device_ptr<T> th_Cm_start(_Cm_dev);
	thrust::device_ptr<T> th_Cm_end (_Cm_dev+vectorSize);

	thrust::device_ptr<T> th_Vm_start(_Vm_dev);
	thrust::device_ptr<T> th_Vm_end (_Vm_dev+vectorSize);

	thrust::device_ptr<T> th_Rm_start(_Rm_dev);
	thrust::device_ptr<T> th_Rm_end (_Rm_dev+vectorSize);

	thrust::for_each(
		thrust::make_zip_iterator( thrust::make_tuple( th_rhs_start	, th_Vm_start 	,th_Cm_start, th_Rm_start) ),
		thrust::make_zip_iterator( thrust::make_tuple( th_rhs_end	, th_Vm_end		,th_Cm_end 	, th_Rm_end) ),
		update_rhs_functor< T >( _dt ) );
//
	getVector(vectorSize, _rhs,_rhs_dev); //TODO maybe is not necessary

	return NO_ERROR;
}

template <typename T, int arch>
void PN2S_SolverComps<T,arch>::makeHinesMatrix(PN2SModel *model, T * matrix)
{
	/*
	 * Some convenience variables
	 */
	vector< double > CmByDt(nComp);
	vector< double > Ga(nComp);
	for ( unsigned int i = 0; i < nComp; i++ ) {
		CmByDt[i] = model->compts[ i ].Cm / ( _dt / 2.0 ) ;
		Ga[i] =  2.0 / model->compts[ i ].Ra ;
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
		matrix[ i * nComp + i ] = (T)(CmByDt[ i ] + 1.0 / model->compts[ i ].Rm);


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


template class PN2S_SolverComps<double, ARCH_SM30>;
