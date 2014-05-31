///////////////////////////////////////////////////////////
//  NetworkAnalyzer.h
//  Implementation of the Class NetworkAnalyzer
//  Created on:      30-Dec-2013 4:04:20 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(AA2911C45_CDD0_4e09_A1A2_A5363E6EF36B__INCLUDED_)
#define AA2911C45_CDD0_4e09_A1A2_A5363E6EF36B__INCLUDED_

#include "../headers.h"
#include "models/Model.h"

namespace pn2s
{

template <typename T, int arch>
class NetworkAnalyzer
{

public:
	uint nComp;
	uint nModel;
	NetworkAnalyzer();
	virtual ~NetworkAnalyzer();
	Error_PN2S ImportNetwork(vector<models::Model <T> > &network);

	vector<models::Compartment<T> *> allCompartments;
	vector<models::HHChannel<T> *> allHHChannels;
private:
	Error_PN2S importCompts(vector<models::Compartment<T> > &cmpts);
	Error_PN2S importHHChannels(vector<models::HHChannel<T> > &chs);

};
}
#endif // !defined(AA2911C45_CDD0_4e09_A1A2_A5363E6EF36B__INCLUDED_)
