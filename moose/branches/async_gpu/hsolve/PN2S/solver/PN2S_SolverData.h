///////////////////////////////////////////////////////////
//  PN2S_SolverData.h
//  Implementation of the Classes related to PN2S_Solver
//  Created on:      27-Dec-2013 7:29:01 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_)
#define A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_
#include "PN2S_SolverChannels.h"
#include "PN2S_SolverComps.h"
#include "PN2S_NetworkAnalyzer.h"

class PN2S_SolverData
{
public:
	PN2S_SolverData();
	virtual ~PN2S_SolverData();

	double GetDt(){ return _dt;}
	void SetDt(double dt){ _dt = dt; _compsSolver.SetDt(dt);}

	hscError PrepareSolver(vector<PN2SModel > &models);

	hscError Process();
//	vector<vector<PN2SModel_Base> > models; //TODO: Encapsulation

	PN2S_SolverComps<double,ARCH_SM30> _compsSolver; //TODO Encapsulation
private:
	double _dt;
	PN2S_NetworkAnalyzer _analyzer;
	PN2S_SolverChannels _channelSolver;

};
#endif // !defined(A73EC9AE3_8C2D_45e3_AE47_0F3CC8B2E661__INCLUDED_)
