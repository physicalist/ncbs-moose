///////////////////////////////////////////////////////////
//  PN2S_SolverData.h
//  Implementation of the Classes related to PN2S_Solver
//  Created on:      27-Dec-2013 7:29:01 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_)
#define A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_
#include "solvers/PN2S_SolverChannels.h"
#include "solvers/PN2S_SolverComps.h"
#include "PN2S_NetworkAnalyzer.h"

template <typename T, int arch>
class PN2S_ModelPack
{
public:
	PN2S_ModelPack();
	virtual ~PN2S_ModelPack();

	double GetDt(){ return _dt;}
	void SetDt(double dt){ _dt = dt; _compsSolver.SetDt(dt); }

	Error_PN2S Reinit(vector<PN2SModel<T,arch> > &models);

	Error_PN2S Process();
	Error_PN2S Output();
	Error_PN2S Input();
//	vector<vector<PN2SModel_Base> > models; //TODO: Encapsulation

	PN2S_SolverComps<CURRENT_TYPE,CURRENT_ARCH> _compsSolver; //TODO Encapsulation
private:
	double _dt;
	PN2S_NetworkAnalyzer<T,arch> _analyzer;
	PN2S_SolverChannels<T,arch> _channelSolver;

};


#endif // !defined(A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_)
