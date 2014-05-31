///////////////////////////////////////////////////////////
//  SolverChannels.h
//  Implementation of the Class SolverChannels
//  Created on:      27-Dec-2013 4:23:16 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A8661C97F_679E_4bb9_84D8_5EEB3718169D__INCLUDED_)
#define A8661C97F_679E_4bb9_84D8_5EEB3718169D__INCLUDED_
#include "../../headers.h"
#include "../models/Model.h"
#include "../NetworkAnalyzer.h"

namespace pn2s
{
namespace solvers
{

template <typename T, int arch>
class SolverChannels
{
private:
	float* hostMemory;
	float* deviceMemory;

public:
	SolverChannels();
	virtual ~SolverChannels();

//	Error_PN2S PrepareSolver(vector<models::Model<T,arch> > &models, NetworkAnalyzer<T,arch> &analyzer);
};

}
}
#endif // !defined(A8661C97F_679E_4bb9_84D8_5EEB3718169D__INCLUDED_)
