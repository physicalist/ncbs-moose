///////////////////////////////////////////////////////////
//  PN2S_NetworkAnalyzer.cpp
//  Implementation of the Class PN2S_NetworkAnalyzer
//  Created on:      30-Dec-2013 4:04:20 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_NetworkAnalyzer.h"
#include <assert.h>

template <typename T, int arch>
PN2S_NetworkAnalyzer<T,arch>::PN2S_NetworkAnalyzer(){
	allCompartments.clear();
	allHHChannels.clear();
	nComp = 0;
}

template <typename T, int arch>
PN2S_NetworkAnalyzer<T,arch>::~PN2S_NetworkAnalyzer(){

}

template <typename T, int arch>
Error_PN2S PN2S_NetworkAnalyzer<T,arch>::ImportNetwork(vector<PN2SModel<T,arch> > &network){
	Error_PN2S res;
	nModel = network.size();
	if( nModel >0)
		nComp = network[0].compts.size();

	typename vector<PN2SModel<T,arch> >::iterator n;
	for( n = network.begin(); n != network.end(); ++n) {
		res = importCompts(n->compts);
		assert(res==Error_PN2S::NO_ERROR);
		res = importHHChannels(n->hhChannels);
		assert(res==Error_PN2S::NO_ERROR);
	}
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S PN2S_NetworkAnalyzer<T,arch>::importCompts(vector<PN2SModel_Compartment<T,arch> > &cmpts)
{
	typename vector<PN2SModel_Compartment<T,arch> >::iterator n;

	for(n = cmpts.begin(); n != cmpts.end(); ++n) {
		allCompartments.push_back(n.base());
		importHHChannels(n->hhchannels);
	}
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S PN2S_NetworkAnalyzer<T,arch>::importHHChannels(vector<PN2SModel_HHChannel<T,arch> > &chs)
{
	if(chs.size() > 0)
	{
		typename vector<PN2SModel_HHChannel<T,arch> >::iterator n;
		for(n = chs.begin(); n != chs.end(); ++n) {
			allHHChannels.push_back(n.base());
		}
	}
	return Error_PN2S::NO_ERROR;
}

template class PN2S_NetworkAnalyzer<double, ARCH_SM30>;
