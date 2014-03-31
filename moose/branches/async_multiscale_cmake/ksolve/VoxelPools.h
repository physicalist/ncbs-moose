/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _VOXEL_POOLS_H
#define _VOXEL_POOLS_H

#include "header.h"
#ifdef USE_GSL
#include "gsl/gsl_errno.h"
#include "gsl/gsl_matrix.h"
#include "gsl/gsl_odeiv2.h"
#endif

#include "OdeSystem.h"
#include "VoxelPoolsBase.h"
#include "RateTerm.h"
#include "FuncTerm.h"
#include "SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "Stoich.h"


class Stoich;
class VoxelPools: public VoxelPoolsBase
{
	public: 
		VoxelPools();
		virtual ~VoxelPools();

		//////////////////////////////////////////////////////////////////
		// Solver interface functions
		//////////////////////////////////////////////////////////////////
		void setStoich( const Stoich* stoich, const OdeSystem* ode );
		void advance( const ProcInfo* p );

		/// This is the function which evaluates the rates.
		static int gslFunc( double t, const double* y, double *dydt, 
						void* params );

	private:
#ifdef USE_GSL
		gsl_odeiv2_driver* driver_;
		gsl_odeiv2_system sys_;
#endif
};

#endif	// _VOXEL_POOLS_H
