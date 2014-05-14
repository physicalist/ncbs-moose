///////////////////////////////////////////////////////////
//  PN2S_SolverComps.h
//  Implementation of the Class PN2S_SolverComps
//  Created on:      27-Dec-2013 7:57:50 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(EA_ABB95B66_E531_4681_AE2B_D1CE4B940FF6__INCLUDED_)
#define EA_ABB95B66_E531_4681_AE2B_D1CE4B940FF6__INCLUDED_

#include "../Definitions.h"
#include "../core/PN2SModel.h"
#include "PN2S_NetworkAnalyzer.h"


template <typename T, int arch>
class PN2S_SolverComps
{
private:
	double _dt;
	uint nModel;
	uint nComp;

	//CUDA variables

	//CUDA variables

		T *_hm; 	// A pointer to Hines Matrices
		T *_hm_dev; // A pointer to Hines Matrices
		T *_rhs; 	// Right hand side of the equation
		T *_rhs_dev;// Right hand side of the equation

		T *_Vm; 	// Vm of the compartments
		T *_Vm_dev; // Vm of the compartments

		T *_Cm; 	// Cm of the compartments
		T *_Cm_dev; // Cm of the compartments

		T *_Em; 	// Em of the compartments
		T *_Em_dev; // Em of the compartments

		T *_Rm; 	// Rm of the compartments
		T *_Rm_dev; // Rm of the compartments

	void  makeHinesMatrix(PN2SModel<T,arch> *model, T * matrix);// float** matrix, uint nCompt);

public:
	PN2S_SolverComps();
	~PN2S_SolverComps();
	hscError PrepareSolver(vector< PN2SModel<T,arch> > &models, PN2S_NetworkAnalyzer<T,arch> &analyzer);
	hscError Process();
	hscError UpdateMatrix();

	double GetDt(){ return _dt;}
	void SetDt(double dt){ _dt = dt;}

	T GetA(int n,int i, int j){return _hm[n*nComp*nComp+i*nComp+j];}
	T GetRHS(int n,int i){return _rhs[n*nComp+i];}
	T GetVm(int n,int i){return _Vm[n*nComp+i];}
	T GetCm(int n,int i){return _Cm[n*nComp+i];}
	T GetRm(int n,int i){return _Rm[n*nComp+i];}
	T GetEm(int n,int i){return _Em[n*nComp+i];}


};
#endif // !defined(EA_ABB95B66_E531_4681_AE2B_D1CE4B940FF6__INCLUDED_)
