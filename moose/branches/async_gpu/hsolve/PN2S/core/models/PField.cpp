///////////////////////////////////////////////////////////
//  SolverData.cpp
//  Implementation of the Class SolverData
//  Created on:      27-Dec-2013 7:29:01 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PField.h"

#include <assert.h>

using namespace pn2s::models;

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
__inline__ Error_PN2S sendVector(uint size, T* h, T* d, cudaStream_t stream)
{
//	cudaError_t stat = cudaMemcpyAsync(d, h, sizeof(h[0])*size, cudaMemcpyHostToDevice, stream);
	cublasStatus_t stat = cublasSetVectorAsync(size, sizeof(h[0]),h, 1,d,1,stream);
//	if (stat != CUDA_SUCCESS) {
//			cerr << "CUDA setting Variables\n";
//			return Error_PN2S::CUDA_Error;
//		}
	return Error_PN2S::NO_ERROR;
}

template <typename T>
__inline__ Error_PN2S getVector(uint size, T* h, T* d, cudaStream_t stream)
{
	cublasStatus_t stat = cublasGetVectorAsync(size, sizeof(h[0]),d, 1,h,1,stream);
//	cudaError_t stat = cudaMemcpyAsync(h, d, sizeof(h[0])*size, cudaMemcpyDeviceToHost, stream);
//	if (stat != CUDA_SUCCESS) {
//		cerr << "CUDA setting Variables\n";
//		return Error_PN2S::CUDA_Error;
//	}
	return Error_PN2S::NO_ERROR;
}


template <typename T, int arch>
PField<T,arch>::PField():
	fieldType(TYPE_IO),
	host(0),
	device(0),
	host_inc(1),
	device_inc(1),
	_size(0)
{

}

template <typename T, int arch>
PField<T,arch>::~PField(){
	//Driver shutting down, so not necessary to release memory
}


template <typename T, int arch>
Error_PN2S PField<T,arch>::AllocateMemory(size_t size)
{
	_size = size;
	CUDA_SAFE_CALL(cudaMallocHost((void **) &host, size * sizeof(host[0]))); //Define pinned memories
	CUDA_SAFE_CALL(cudaMalloc((void **) &device, size * sizeof(device[0])));

	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S PField<T,arch>::Host2Device_Async(cudaStream_t stream)
{
	CALL(sendVector<T>(_size, host,device, stream));
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S PField<T,arch>::Device2Host_Async(cudaStream_t stream)
{
	CALL(getVector<T>(_size, host,device,stream));
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S PField<T,arch>::Send2Device_Async(PField& _hostResource,cudaStream_t stream)
{
	size_t minSize = min(_hostResource._size, _size);
	if(minSize > 0)
		CALL(sendVector<T>(minSize, _hostResource.host,device,stream));
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S PField<T,arch>::Send2Host_Async(PField& _hostResource,cudaStream_t stream)
{
	size_t minSize = min(_hostResource._size, _size);
	if(minSize > 0)
		CALL(getVector<T>(minSize, _hostResource.host,device,stream));
	return Error_PN2S::NO_ERROR;
}

template class PField<double, ARCH_SM30>;
