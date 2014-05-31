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
#include "../NetworkAnalyzer.h"
#include "Field.h"

namespace pn2s
{
namespace solvers
{

template <typename T, int arch>
class SolverComps
{
private:
	double _dt;
	uint nModel;
	uint n;
	vector<uint> _ids;

	//Connection Fields
	Field<T, arch>  _mxs;	// Matrices
	Field<T, arch>  _rhs;	// Right hand side of the equation
	Field<T, arch>  _Xs;	// Vm of the compartments

	void  makeHinesMatrix(models::Model<T> *model, T * matrix);// float** matrix, uint nCompt);
	void getValues();
public:
	SolverComps();
	~SolverComps();
	Error_PN2S PrepareSolver(vector< models::Model<T> > &models, NetworkAnalyzer<T,arch> &analyzer);
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

}
}
#endif // !defined(EA_ABB95B66_E531_4681_AE2B_D1CE4B940FF6__INCLUDED_)
