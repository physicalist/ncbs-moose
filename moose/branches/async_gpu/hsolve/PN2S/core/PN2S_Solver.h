///////////////////////////////////////////////////////////
//  PN2S_Solver.h
//  Implementation of the Class PN2S_Solver
//  Created on:      26-Dec-2013 4:21:51 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(FCD33A96_9E58_4bec_BA66_91CF4FD383BD__INCLUDED_)
#define FCD33A96_9E58_4bec_BA66_91CF4FD383BD__INCLUDED_

#include "../PN2S.h"
#include "PN2S_ModelPack.h"
#include "solvers/PN2S_SolverChannels.h"
#include "solvers/PN2S_SolverComps.h"
#include "models/PN2SModel.h"

template <typename T, int arch>
class PN2S_Solver
{
private:
	double _dt;
	map<uint, PN2S_ModelPack<T,arch>*> _modelToPackMap;
public:
	vector<PN2S_ModelPack<T,arch> > modelPacks;

	PN2S_Solver();
	virtual ~PN2S_Solver();

	double GetDt(){ return _dt;}
	void SetDt(double dt){ _dt = dt;}


	Error_PN2S Setup(double dt);
	Error_PN2S PrepareSolver( vector<PN2SModel<T,arch> > & _models,  double dt);
	PN2S_ModelPack<T,arch>* FindModelPack(hscID_t id);
	void Process(PN2S_ModelPack<T,arch>* data);
};

#endif // !defined(FCD33A96_9E58_4bec_BA66_91CF4FD383BD__INCLUDED_)
