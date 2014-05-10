///////////////////////////////////////////////////////////
//  PN2S_Solver.h
//  Implementation of the Class PN2S_Solver
//  Created on:      26-Dec-2013 4:21:51 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(FCD33A96_9E58_4bec_BA66_91CF4FD383BD__INCLUDED_)
#define FCD33A96_9E58_4bec_BA66_91CF4FD383BD__INCLUDED_

#include "../Definitions.h"
#include "PN2S_SolverData.h"
#include "PN2S_SolverChannels.h"
#include "PN2S_SolverComps.h"
#include "../model/PN2SModel.h"
#include "../PN2S_Device.h"

class PN2S_Solver
{
private:
	double _dt;
	map<uint, PN2S_SolverData*> _modelToPackMap;
public:
	vector<PN2S_SolverData> solverPacks;

	PN2S_Solver();
	virtual ~PN2S_Solver();

	double GetDt(){ return _dt;}
	void SetDt(double dt){ _dt = dt;}


	hscError Setup(double dt);
	hscError PrepareSolver( vector<PN2SModel> & _models);
	PN2S_SolverData* LocateDataByID(hscID_t id);
	void Process(PN2S_SolverData* data, PN2S_Device* d);
};

#endif // !defined(FCD33A96_9E58_4bec_BA66_91CF4FD383BD__INCLUDED_)
