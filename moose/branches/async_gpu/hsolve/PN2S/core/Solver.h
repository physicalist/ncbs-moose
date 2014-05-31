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
#include "solvers/SolverChannels.h"
#include "solvers/SolverComps.h"
#include "models/Model.h"

namespace pn2s
{

template <typename T, int arch>
class Solver
{
private:
	double _dt;
	map<uint, ModelPack<T,arch>*> _modelToPackMap;
public:
	vector<ModelPack<T,arch> > modelPacks;

	Solver();
	virtual ~Solver();

	double GetDt(){ return _dt;}
	void SetDt(double dt){ _dt = dt;}


	Error_PN2S Setup(double dt);
	Error_PN2S PrepareSolver( vector<models::Model<T> > & _models,  double dt);
	ModelPack<T,arch>* FindModelPack(hscID_t id);
	void Process(ModelPack<T,arch>* data);
};

}
#endif // !defined(FCD33A96_9E58_4bec_BA66_91CF4FD383BD__INCLUDED_)
