///////////////////////////////////////////////////////////
//  NetworkAnalyzer.cpp
//  Implementation of the Class NetworkAnalyzer
//  Created on:      30-Dec-2013 4:04:20 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "NetworkAnalyzer.h"
#include <assert.h>

using namespace pn2s;

NetworkAnalyzer::NetworkAnalyzer(){
	allCompartments.clear();
	allHHChannels.clear();
	nComp = 0;
}

NetworkAnalyzer::~NetworkAnalyzer(){

}

Error_PN2S NetworkAnalyzer::ImportNetwork(vector<models::Model> &network){
	Error_PN2S res;
	nModel = network.size();
	if( nModel >0)
		nComp = network[0].compts.size();

	typename vector<models::Model>::iterator n;
	for( n = network.begin(); n != network.end(); ++n) {
		res = importCompts(n->compts);
		assert(res==Error_PN2S::NO_ERROR);
//		res = importHHChannels(n->hhChannels);
		assert(res==Error_PN2S::NO_ERROR);
	}
	return Error_PN2S::NO_ERROR;
}

Error_PN2S NetworkAnalyzer::importCompts(vector<models::Compartment > &cmpts)
{
	typename vector<models::Compartment >::iterator n;

	for(n = cmpts.begin(); n != cmpts.end(); ++n) {
		allCompartments.push_back(n.base());
//		importHHChannels(n->hhchannels);
	}
	return Error_PN2S::NO_ERROR;
}

Error_PN2S NetworkAnalyzer::importHHChannels(vector<models::HHChannel > &chs)
{
	if(chs.size() > 0)
	{
		typename vector<models::HHChannel >::iterator n;
		for(n = chs.begin(); n != chs.end(); ++n) {
			allHHChannels.push_back(n.base());
		}
	}
	return Error_PN2S::NO_ERROR;
}

