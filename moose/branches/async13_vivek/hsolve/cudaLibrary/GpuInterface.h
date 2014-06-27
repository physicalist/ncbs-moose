/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _GPU_INTERFACE_H
#define _GPU_INTERFACE_H

#include "../../basecode/header.h"			// For miscellaneous defns
#include "../HSolveStruct.h"				// For structure definitions
#include "../RateLookup.h"
#include "../HinesMatrix.h"					// For JunctionStruct
#include "../HSolvePassive.h"
#include "../HSolveActive.h"
#include "../HSolve.h"						// For HSolve
//#include "../testGpuInterface.h"			// For testing this class

/**
 * GpuInterface class that allows each Hines Solver to create its own interface
 * with the GPU.
 */
class GpuInterface {

	public:
		GpuInterface();
		void makeOperands();
		void gpuUpdateMatrix();
		void gpuForwardEliminate();
		void gpuBackwardSubstitute();
};

#endif
