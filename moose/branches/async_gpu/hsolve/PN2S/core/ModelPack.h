///////////////////////////////////////////////////////////
//  SolverData.h
//  Implementation of the Classes related to Solver
//  Created on:      27-Dec-2013 7:29:01 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_)
#define A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_
#include "solvers/SolverChannels.h"
#include "solvers/SolverComps.h"
#include "NetworkAnalyzer.h"

namespace pn2s
{

template <typename T, int arch>
class ModelPack
{
public:
	ModelPack();
	virtual ~ModelPack();

	double GetDt(){ return _dt;}
	void SetDt(double dt){ _dt = dt; _compsSolver.SetDt(dt); }

	Error_PN2S Reinit(vector<models::Model<T> > &models);

	Error_PN2S Process();
	Error_PN2S Output();
	Error_PN2S Input();
//	vector<vector<models::Base> > models; //TODO: Encapsulation

	solvers::SolverComps<CURRENT_TYPE,CURRENT_ARCH> _compsSolver; //TODO Encapsulation
	solvers::SolverChannels<CURRENT_TYPE,CURRENT_ARCH> _channelsSolver; //TODO Encapsulation
private:
	double _dt;
	NetworkAnalyzer<T,arch> _analyzer;

};

}

#endif // !defined(A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_)
