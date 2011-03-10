/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _GSL_INTEGRATOR_H
#define _GSL_INTEGRATOR_H
class GslIntegrator
{
	public:
		GslIntegrator();
		~GslIntegrator();

///////////////////////////////////////////////////
// Field function definitions
///////////////////////////////////////////////////
		bool getIsInitialized() const;
		string getMethod() const;
		void setMethod( string method );
		double getRelativeAccuracy() const;
		void setRelativeAccuracy( double value );
		double getAbsoluteAccuracy() const;
		void setAbsoluteAccuracy( double value );
		double getInternalDt() const;
		void setInternalDt( double value );

///////////////////////////////////////////////////
// Dest function definitions
///////////////////////////////////////////////////

		void process( const Eref& e, ProcPtr info );
		void reinit( const Eref& e, ProcPtr info );

		void stoich( Id stoichId );

		static const Cinfo* initCinfo();
	private:
		bool isInitialized_;
		string method_;
		double absAccuracy_;
		double relAccuracy_;
		double internalStepSize_;
		double* y_;
		unsigned int nVarMols_;
		void* stoich_;
//		const vector< unsigned int >* dynamicBuffers_;

		const gsl_odeiv_step_type* gslStepType_;
		gsl_odeiv_step* gslStep_;
		gsl_odeiv_control* gslControl_;
		gsl_odeiv_evolve* gslEvolve_;
		gsl_odeiv_system gslSys_;
};
#endif // _GSL_INTEGRATOR_H
