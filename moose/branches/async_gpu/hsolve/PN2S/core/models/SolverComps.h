///////////////////////////////////////////////////////////
//  SolverComps.h
//  Implementation of the Class SolverComps
//  Created on:      27-Dec-2013 7:57:50 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(EA_ABB95B66_E531_4681_AE2B_D1CE4B940FF6__INCLUDED_)
#define EA_ABB95B66_E531_4681_AE2B_D1CE4B940FF6__INCLUDED_

#include "../../headers.h"
#include "../models/Model.h"
#include "PField.h"
#include "ModelStatistic.h"

namespace pn2s
{
namespace models
{

class SolverComps
{
private:
	ModelStatistic _stat;
	Model* _models;

	//Connection Fields
	PField<TYPE_, ARCH_>  _hm;	// Hines Matrices
	PField<TYPE_, ARCH_>  _rhs;	// Right hand side of the equation
	PField<TYPE_, ARCH_>  _Vm;	// Vm of the compartments
	PField<TYPE_, ARCH_>  _Cm;	// Cm of the compartments
	PField<TYPE_, ARCH_>  _Em;	// Em of the compartments
	PField<TYPE_, ARCH_>  _Rm;	// Rm of the compartments
	PField<TYPE_, ARCH_>  _Ra;	// Ra of the compartments

	void  makeHinesMatrix(models::Model *model, TYPE_ * matrix);// float** matrix, uint nCompt);
	void getValues();
public:
	SolverComps();
	~SolverComps();
	Error_PN2S AllocateMemory(models::Model * m, models::ModelStatistic& s);
	Error_PN2S PrepareSolver();
	Error_PN2S Input();
	Error_PN2S Process();
	Error_PN2S Output();
	Error_PN2S UpdateMatrix();

	double GetDt(){ return _stat.dt;}
	void SetDt(double dt){ _stat.dt = dt;}

//	TYPE_ GetA(int n,int i, int j){return _hm[n*nComp*nComp+i*nComp+j];}
//	TYPE_ GetRHS(int n,int i){return _rhs[n*nComp+i];}
//	static TYPE_ GetVm(int n,int i){return _Vm[n*nComp+i];}
	void 	SetValue(Compartment* c, FIELD::TYPE field, TYPE_ value);
	TYPE_ 	GetValue(Compartment* c, FIELD::TYPE field);
//	TYPE_ GetCm(int n,int i){return _Cm[n*nComp+i];}
//	TYPE_ GetRm(int n,int i){return _Rm[n*nComp+i];}
//	TYPE_ GetEm(int n,int i){return _Em[n*nComp+i];}

	static TYPE_ (*Fetch_Func) (uint id, FIELD::TYPE field);
};

}
}
#endif // !defined(EA_ABB95B66_E531_4681_AE2B_D1CE4B940FF6__INCLUDED_)
