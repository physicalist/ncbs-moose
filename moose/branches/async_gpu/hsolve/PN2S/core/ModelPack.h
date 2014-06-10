///////////////////////////////////////////////////////////
//  SolverData.h
//  Implementation of the Classes related to Solver
//  Created on:      27-Dec-2013 7:29:01 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_)
#define A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_
#include "models/SolverChannels.h"
#include "models/SolverComps.h"
#include "NetworkAnalyzer.h"

namespace pn2s
{

class ModelPack
{
public:
	ModelPack();
	virtual ~ModelPack();

	double GetDt(){ return _dt;}
	void SetDt(double dt){ _dt = dt;
//	_compsSolver.SetDt(dt);
	}

	Error_PN2S Reinit(vector<models::Model > &models);

	Error_PN2S Process();
	Error_PN2S Output();
	Error_PN2S Input();
//	vector<vector<models::Base> > models; //TODO: Encapsulation

//	solvers::SolverComps<TYPE_,CURRENT_ARCH> _compsSolver; //TODO Encapsulation
//	solvers::SolverChannels<TYPE_,CURRENT_ARCH> _channelsSolver; //TODO Encapsulation

//	friend ostream& operator<<(ostream& out, ModelPack<T,arch>& dt)
//	{
//	    out << "ModelPakc";
//	    return out;
//	}

private:
	double _dt;
	NetworkAnalyzer _analyzer;

};

}

#endif // !defined(A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_)
