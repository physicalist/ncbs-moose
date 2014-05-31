///////////////////////////////////////////////////////////
//  NetworkAnalyzer.cpp
//  Implementation of the Class NetworkAnalyzer
//  Created on:      30-Dec-2013 4:04:20 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "NetworkAnalyzer.h"
#include <assert.h>

using namespace pn2s;

template <typename T, int arch>
NetworkAnalyzer<T,arch>::NetworkAnalyzer(){
	allCompartments.clear();
	allHHChannels.clear();
	nComp = 0;
}

template <typename T, int arch>
NetworkAnalyzer<T,arch>::~NetworkAnalyzer(){

}

template <typename T, int arch>
Error_PN2S NetworkAnalyzer<T,arch>::ImportNetwork(vector<models::Model<T> > &network){
	Error_PN2S res;
	nModel = network.size();
	if( nModel >0)
		nComp = network[0].compts.size();

	typename vector<models::Model<T> >::iterator n;
	for( n = network.begin(); n != network.end(); ++n) {
		res = importCompts(n->compts);
		assert(res==Error_PN2S::NO_ERROR);
		res = importHHChannels(n->hhChannels);
		assert(res==Error_PN2S::NO_ERROR);
	}
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S NetworkAnalyzer<T,arch>::importCompts(vector<models::Compartment<T> > &cmpts)
{
	typename vector<models::Compartment<T> >::iterator n;

	for(n = cmpts.begin(); n != cmpts.end(); ++n) {
		allCompartments.push_back(n.base());
		importHHChannels(n->hhchannels);
	}
	return Error_PN2S::NO_ERROR;
}

template <typename T, int arch>
Error_PN2S NetworkAnalyzer<T,arch>::importHHChannels(vector<models::HHChannel<T> > &chs)
{
	if(chs.size() > 0)
	{
		typename vector<models::HHChannel<T> >::iterator n;
		for(n = chs.begin(); n != chs.end(); ++n) {
			allHHChannels.push_back(n.base());
		}
	}
	return Error_PN2S::NO_ERROR;
}

template class NetworkAnalyzer<double, ARCH_SM30>;
