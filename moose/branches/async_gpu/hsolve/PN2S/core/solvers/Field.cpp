///////////////////////////////////////////////////////////
//  SolverData.cpp
//  Implementation of the Class SolverData
//  Created on:      27-Dec-2013 7:29:01 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "Field.h"

#include <assert.h>

using namespace pn2s::solvers;

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
__inline__ Error_PN2S sendVector(uint size, T* h, T* d)
{
	cublasStatus_t stat = cublasSetVector(size, sizeof(h[0]),h, 1,d,1);
	if (stat != CUBLAS_STATUS_SUCCESS) {
		cerr << "CUBLAS setting Variables\n";
		return Error_PN2S::CuBLASError;
	}
	return Error_PN2S::NO_ERROR;
}

template <typename T>
__inline__ Error_PN2S getVector(uint size, T* h, T* d)
{
	cublasStatus_t stat = cublasGetVector(size, sizeof(h[0]),d, 1,h,1);
	if (stat != CUBLAS_STATUS_SUCCESS) {
		cerr << "CUBLAS setting Variables\n";
		return Error_PN2S::CuBLASError;
	}
	return Error_PN2S::NO_ERROR;
}


template <typename T, int arch>
Field<T,arch>::Field():
	fieldType(TYPE_IO),
	host(0),
	device(0),
	host_inc(1),
	device_inc(1),
	_size(0)
{

}

//template <typename T, int arch>
//Field<T,arch>::Field(FieldType t):
//	fieldType(t),
//	host(0),
//	device(0),
//	host_inc(1),
//	device_inc(1)
//{
//
//}

template <typename T, int arch>
Field<T,arch>::~Field(){
	//TODO: Remove!
}


template <typename T, int arch>
Error_PN2S Field<T,arch>::AllocateMemory(size_t size)
{
	_size = size;
	CUDA_SAFE_CALL(cudaMallocHost((void **) &host, size * sizeof(host[0]))); //Define pinned memories
	CUDA_SAFE_CALL(cudaMalloc((void **) &device, size * sizeof(device[0])));

	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S Field<T,arch>::Host2Device_Sync()
{
	CALL(sendVector<T>(_size, host,device));
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S Field<T,arch>::Device2Host_Sync()
{
	CALL(getVector<T>(_size, host,device));
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S Field<T,arch>::Send2Device(Field& _hostResource)
{
	size_t minSize = min(_hostResource._size, _size);
	if(minSize > 0)
		CALL(sendVector<T>(minSize, _hostResource.host,device));
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S Field<T,arch>::Send2Host(Field& _hostResource)
{
	size_t minSize = min(_hostResource._size, _size);
	if(minSize > 0)
		CALL(getVector<T>(minSize, _hostResource.host,device));
	return Error_PN2S::NO_ERROR;
}

template class Field<double, ARCH_SM30>;
