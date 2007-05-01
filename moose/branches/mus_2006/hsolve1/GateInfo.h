/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _GateInfo_h
#define _GateInfo_h

#include "HSolveModel.h"
#include <limits>
#include <cmath>

template < class T >
class GateInfo
{
public:
	GateInfo( HSolveModel< T >* model, const T& gid )
		: model_( model ), gid_( gid )
	{
		model->getFieldDouble( gid, "power", power_ );
		model->getFieldDouble( gid, "state", state_ );
		model->getFieldInt( gid, "alphaForm", alphaForm_ );
		model->getFieldInt( gid, "betaForm", betaForm_ );
		model->getFieldDouble( gid, "alpha_A", alpha_A_ );
		model->getFieldDouble( gid, "alpha_B", alpha_B_ );
		model->getFieldDouble( gid, "alpha_V0", alpha_V0_ );
		model->getFieldDouble( gid, "beta_A", beta_A_ );
		model->getFieldDouble( gid, "beta_B", beta_B_ );
		model->getFieldDouble( gid, "beta_V0", beta_V0_ );
	}
	
	bool operator< ( const GateInfo& right ) const {
		if ( alphaForm_ != right.alphaForm_ )
			return ( alphaForm_ < right.alphaForm_ );
		else if ( betaForm_ != right.betaForm_ )
			return ( betaForm_ < right.betaForm_ );
		else if ( !equals( alpha_A_, right.alpha_A_ ) )
			return ( alpha_A_ < right.alpha_A_ );
		else if ( !equals( alpha_B_, right.alpha_B_ ) )
			return ( alpha_B_ < right.alpha_B_ );
		else if ( !equals( alpha_V0_, right.alpha_V0_ ) )
			return ( alpha_V0_ < right.alpha_V0_ );
		else if ( !equals( beta_A_, right.beta_A_ ) )
			return ( beta_A_ < right.beta_A_ );
		else if ( !equals( beta_B_, right.beta_B_ ) )
			return ( beta_B_ < right.beta_B_ );
		else
			return ( beta_V0_ < right.beta_V0_ );
	}

	double alpha( double V ) const {
		return model_->getAlpha( gid_, V );
	}
	
	double beta( double V ) const {
		return model_->getBeta( gid_, V );
	}
	
	double state() const {
		return state_;
	}
	
	double power() const {
		return power_;
	}
	
private:
	HSolveModel< T >* model_;
	T gid_;
	double power_;
	double state_;
	int alphaForm_;
	double alpha_A_;
	double alpha_B_;
	double alpha_V0_;
	int betaForm_;
	double beta_A_;
	double beta_B_;
	double beta_V0_;
	
	static bool equals( double a, double b ) {
		return fabs( 1 - b / a ) < numeric_limits< double >::epsilon();
	}
};

#endif
