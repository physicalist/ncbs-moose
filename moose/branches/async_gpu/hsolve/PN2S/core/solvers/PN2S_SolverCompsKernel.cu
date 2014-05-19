#include <stdio.h>

#include <cuda_runtime.h>
#include "PN2S_SolverComps.h"
/**
 * CUDA Kernel Device code
 *
 * Computes the Matrix Update via this equation:
 *  Rhs = Vm * Cm / (dt / 2.0) + Em / Rm
 *
 */
template<typename T, int arch>
__global__ void
UpdateRHS(const float *A, const float *B, float *rhs, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

//    if (i < numElements)
//    {
//        C[i] = A[i] + B[i];
//    }
}

// C Wrapper
Error_PN2S UpdateRHS (double *A, double *b, double *x, int n, int batch)
{
//    dim3 dimBlock(dimX[n], n+1);
//    dim3 dimGrid;
//    if (batch <= GRID_DIM_LIMIT) {
//        dimGrid.x = batch;
//        dimGrid.y = 1;
//        dimGrid.z = 1;
//    } else {
//        dimGrid.x = GRID_DIM_LIMIT;
//        dimGrid.y = (batch + GRID_DIM_LIMIT-1) / GRID_DIM_LIMIT;
//        dimGrid.z = 1;
//    }
//
//	UpdateRHS<double,ARCH_SM30> <<<dimGrid,dimBlock,smem_size>>>(A_d,b_d,x_d,n,batch);
//	cudaError_t err = cudaGetLastError();
//	/* Check synchronous errors, i.e. pre-launch */
//	if (cudaSuccess != err) {
//		return CUDA_Error;

    return NO_ERROR;
}
