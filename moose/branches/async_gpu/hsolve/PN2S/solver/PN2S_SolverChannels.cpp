///////////////////////////////////////////////////////////
//  PN2S_SolverChannels.cpp
//  Implementation of the Class PN2S_SolverChannels
//  Created on:      27-Dec-2013 4:23:16 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_SolverChannels.h"
#include <cuda.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>

#define IDX2C(i,j,ld) (((j)*(ld))+(i))

PN2S_SolverChannels::PN2S_SolverChannels() {

}

PN2S_SolverChannels::~PN2S_SolverChannels() {
//	free(hostMemory);
}

hscError PN2S_SolverChannels::PrepareSolver(vector<PN2SModel> &network, PN2S_NetworkAnalyzer &analyzer) {

//	uint hhSize = 0;
//	uint networkSize = network.size();
//	for (uint i = 0; i < networkSize; i++)
//		hhSize += analyzer.[i].size();
//
//	hostMemory = (float *) malloc(hhSize * sizeof(*hostMemory));
//
//	//Fill data
//	float* dataPointer = hostMemory;
//	for (uint i = 0; i < networkSize; i++) {
//		uint modelSize = network[i].hhChannels.size();
//		for (uint j = 0; j < modelSize; j++) {
//			*dataPointer = network[i].hhChannels[j].Vm;
//			dataPointer++;
//		}
//	}
//
//	cudaMalloc((void**) &deviceMemory, hhSize * sizeof(*deviceMemory));
//
//	cublasHandle_t cublasHandle;
//	cublasStatus_t stat = cublasCreate(&cublasHandle);
//
//	cublasSetVector(hhSize, sizeof(*hostMemory), hostMemory, 1, deviceMemory,1);
//
//	for(float f =1.1; f<10; f+=.001)
//		cublasSscal(cublasHandle, hhSize,&f , deviceMemory, 1);
//
//	cublasGetVector(hhSize, sizeof(*hostMemory), deviceMemory, 1, hostMemory,	1);
//
//	cudaFree(deviceMemory);
//	cublasDestroy(cublasHandle);
//	free(hostMemory);


	return NO_ERROR;
}
