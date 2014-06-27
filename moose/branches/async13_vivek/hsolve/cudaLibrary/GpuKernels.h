/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _GPU_KERNELS_H
#define _GPU_KERNELS_H

#include "GpuInterface.h"

// Function to set constant memory from CPU via the setup function.
void setInstantXYZ(int x, int y, int z);

// HSolvePassive stuff
__global__ void updateMatrixKernel(GpuDataStruct);
__global__ void forwardEliminateKernel(GpuDataStruct);
__global__ void backwardSubstituteKernel(GpuDataStruct);

// HSolveActive stuff
__global__ void advanceChannelsKernel(GpuDataStruct, double);
__global__ void calculateChannelCurrentsKernel(GpuDataStruct);
__global__ void advanceCalciumKernel(GpuDataStruct);
// No need for advanceSynChans, since we will do buffering and get back to the
// CPU for that.

__device__ void findRow(GpuLookupTable, double, GpuLookupRow&);
__device__ void lookupTable(GpuLookupTable, GpuLookupColumn, GpuLookupRow,
							double&, double&);

#endif
