///////////////////////////////////////////////////////////
//  PN2S_SolverComps.h
//  Implementation of the Class PN2S_SolverComps
//  Created on:      27-Dec-2013 7:57:50 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(EA_ABB95B66_E531_4681_AE2B_D1CE4B940FF6__INCLUDED_)
#define EA_ABB95B66_E531_4681_AE2B_D1CE4B940FF6__INCLUDED_

#include "../../PN2S.h"
#include "../models/PN2SModel.h"
#include "../PN2S_NetworkAnalyzer.h"
#include "PN2S_Field.h"

template <typename T, int arch>
class PN2S_SolverComps
{
private:
	double _dt;
	uint nModel;
	uint nComp;
	vector<uint> _ids;

	//Connection Fields
	PN2S_Field<T, arch>  _hm;	// Hines Matrices
	PN2S_Field<T, arch>  _rhs;	// Right hand side of the equation
	PN2S_Field<T, arch>  _Vm;	// Vm of the compartments
	PN2S_Field<T, arch>  _Cm;	// Cm of the compartments
	PN2S_Field<T, arch>  _Em;	// Em of the compartments
	PN2S_Field<T, arch>  _Rm;	// Rm of the compartments

	void  makeHinesMatrix(PN2SModel<T,arch> *model, T * matrix);// float** matrix, uint nCompt);
	void getValues();
public:
	PN2S_SolverComps();
	~PN2S_SolverComps();
	Error_PN2S PrepareSolver(vector< PN2SModel<T,arch> > &models, PN2S_NetworkAnalyzer<T,arch> &analyzer);
	Error_PN2S Process();
	Error_PN2S UpdateMatrix();

	double GetDt(){ return _dt;}
	void SetDt(double dt){ _dt = dt;}

	T GetA(int n,int i, int j){return _hm[n*nComp*nComp+i*nComp+j];}
	T GetRHS(int n,int i){return _rhs[n*nComp+i];}
	T GetVm(int n,int i){return _Vm[n*nComp+i];}
	T GetCm(int n,int i){return _Cm[n*nComp+i];}
	T GetRm(int n,int i){return _Rm[n*nComp+i];}
	T GetEm(int n,int i){return _Em[n*nComp+i];}

	//Setter and Getter
	enum Fields {CM_FIELD, EM_FIELD, RM_FIELD, RA_FIELD,INIT_VM_FIELD, VM_FIELD};

	static T (*GetValue_Func) (uint id, Fields field);
};
#endif // !defined(EA_ABB95B66_E531_4681_AE2B_D1CE4B940FF6__INCLUDED_)
