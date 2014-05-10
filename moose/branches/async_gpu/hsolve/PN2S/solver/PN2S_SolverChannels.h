///////////////////////////////////////////////////////////
//  PN2S_SolverChannels.h
//  Implementation of the Class PN2S_SolverChannels
//  Created on:      27-Dec-2013 4:23:16 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A8661C97F_679E_4bb9_84D8_5EEB3718169D__INCLUDED_)
#define A8661C97F_679E_4bb9_84D8_5EEB3718169D__INCLUDED_
#include "../Definitions.h"
#include "../model/PN2SModel.h"
#include "PN2S_NetworkAnalyzer.h"

class PN2S_SolverChannels
{
private:
	float* hostMemory;
	float* deviceMemory;

public:
	PN2S_SolverChannels();
	virtual ~PN2S_SolverChannels();

	hscError PrepareSolver(vector<PN2SModel> &models, PN2S_NetworkAnalyzer &analyzer);
};
#endif // !defined(A8661C97F_679E_4bb9_84D8_5EEB3718169D__INCLUDED_)
