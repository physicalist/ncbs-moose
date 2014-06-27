#ifndef _GPU_DATA_STRUCT_H
#define _GPU_DATA_STRUCT_H

#include "../HSolveStruct.h"				// For structure definitions
#include "../HinesMatrix.h"					// For JunctionStruct

struct OperandStruct {
	double *ops;
	unsigned int nOps;
};

/* Structure to store data that to be transferred to the GPU */
struct GpuDataStruct {
	double *HS;							// Tridiagonal part of Hines matrix
	double *HJ;							// Off-diagonal elements
	double *V;							// Vm values of compartments
	double *VMid;						// Vm values at mid-time-step
	double *HJCopy;						//
	CompartmentStruct *compartment;		// Array of compartments
	OperandStruct *operand;				// Array of operands
	OperandStruct *backOperand			//
	JunctionStruct *junction;			// Array of junctions

	unsigned int nCompts;
	unsigned int HJSize;

	int stage;
};

#endif
