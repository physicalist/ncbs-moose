///////////////////////////////////////////////////////////
//  Solver.h
//  Implementation of the Class Solver
//  Created on:      26-Dec-2013 4:21:51 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(FCD33A96_9E58_4bec_BA66_91CF4FD383BD__INCLUDED_)
#define FCD33A96_9E58_4bec_BA66_91CF4FD383BD__INCLUDED_

#include "../headers.h"
#include "ModelPack.h"
#include "models/SolverChannels.h"
#include "models/SolverComps.h"
#include "models/Model.h"

namespace pn2s
{

class Solver
{
private:
	double _dt;
	map<uint, ModelPack*> _modelToPackMap;
public:
	vector<ModelPack > modelPacks;

	Solver();
	virtual ~Solver();

	double GetDt(){ return _dt;}
	void SetDt(double dt){ _dt = dt;}


	Error_PN2S Setup(double dt);
	Error_PN2S PrepareSolver( vector<models::Model > & _models,  double dt);
	ModelPack* FindModelPack(hscID_t id);
	void Process(ModelPack* data);
};

}
#endif // !defined(FCD33A96_9E58_4bec_BA66_91CF4FD383BD__INCLUDED_)
