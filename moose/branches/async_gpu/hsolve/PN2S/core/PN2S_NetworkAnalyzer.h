///////////////////////////////////////////////////////////
//  PN2S_NetworkAnalyzer.h
//  Implementation of the Class PN2S_NetworkAnalyzer
//  Created on:      30-Dec-2013 4:04:20 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(AA2911C45_CDD0_4e09_A1A2_A5363E6EF36B__INCLUDED_)
#define AA2911C45_CDD0_4e09_A1A2_A5363E6EF36B__INCLUDED_

#include "../PN2S.h"
#include "models/PN2SModel.h"

template <typename T, int arch>
class PN2S_NetworkAnalyzer
{

public:
	uint nComp;
	uint nModel;
	PN2S_NetworkAnalyzer();
	virtual ~PN2S_NetworkAnalyzer();
	Error_PN2S ImportNetwork(vector<PN2SModel <T,arch> > &network);

	vector<PN2SModel_Compartment<T,arch> *> allCompartments;
	vector<PN2SModel_HHChannel<T,arch> *> allHHChannels;
private:
	Error_PN2S importCompts(vector<PN2SModel_Compartment<T,arch> > &cmpts);
	Error_PN2S importHHChannels(vector<PN2SModel_HHChannel<T,arch> > &chs);

};
#endif // !defined(AA2911C45_CDD0_4e09_A1A2_A5363E6EF36B__INCLUDED_)
