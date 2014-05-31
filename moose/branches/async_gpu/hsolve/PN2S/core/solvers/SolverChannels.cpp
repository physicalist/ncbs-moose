///////////////////////////////////////////////////////////
//  SolverChannels.cpp
//  Implementation of the Class SolverChannels
//  Created on:      27-Dec-2013 4:23:16 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "SolverChannels.h"
#include <cuda.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>

namespace pn2s
{
namespace solvers
{


#define IDX2C(i,j,ld) (((j)*(ld))+(i))

template <typename T, int arch>
SolverChannels<T,arch>::SolverChannels() {

}

template <typename T, int arch>
SolverChannels<T,arch>::~SolverChannels() {
//	free(hostMemory);
}

//template <typename T, int arch>
//Error_PN2S SolverChannels<T,arch>::PrepareSolver(vector<models::Model<T,arch> > &network, NetworkAnalyzer<T,arch> &analyzer) {
//
////	uint hhSize = 0;
////	uint networkSize = network.size();
////	for (uint i = 0; i < networkSize; i++)
////		hhSize += analyzer.[i].size();
////
////	hostMemory = (float *) malloc(hhSize * sizeof(*hostMemory));
////
////	//Fill data
////	float* dataPointer = hostMemory;
////	for (uint i = 0; i < networkSize; i++) {
////		uint modelSize = network[i].hhChannels.size();
////		for (uint j = 0; j < modelSize; j++) {
////			*dataPointer = network[i].hhChannels[j].Vm;
////			dataPointer++;
////		}
////	}
////
////	cudaMalloc((void**) &deviceMemory, hhSize * sizeof(*deviceMemory));
////
////	cublasHandle_t cublasHandle;
////	cublasStatus_t stat = cublasCreate(&cublasHandle);
////
////	cublasSetVector(hhSize, sizeof(*hostMemory), hostMemory, 1, deviceMemory,1);
////
////	for(float f =1.1; f<10; f+=.001)
////		cublasSscal(cublasHandle, hhSize,&f , deviceMemory, 1);
////
////	cublasGetVector(hhSize, sizeof(*hostMemory), deviceMemory, 1, hostMemory,	1);
////
////	cudaFree(deviceMemory);
////	cublasDestroy(cublasHandle);
////	free(hostMemory);
//
//
//	return Error_PN2S::NO_ERROR;
//}

template class SolverChannels<double, ARCH_SM30>;

}
}

