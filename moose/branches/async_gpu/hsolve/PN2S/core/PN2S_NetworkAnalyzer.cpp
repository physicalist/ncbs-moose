///////////////////////////////////////////////////////////
//  PN2S_NetworkAnalyzer.cpp
//  Implementation of the Class PN2S_NetworkAnalyzer
//  Created on:      30-Dec-2013 4:04:20 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_NetworkAnalyzer.h"
#include <assert.h>

PN2S_NetworkAnalyzer::PN2S_NetworkAnalyzer(){
	allCompartments.clear();
	allHHChannels.clear();
	nComp = 0;
}


PN2S_NetworkAnalyzer::~PN2S_NetworkAnalyzer(){

}

hscError PN2S_NetworkAnalyzer::ImportNetwork(vector<PN2SModel> &network){
	hscError res;
	nModel = network.size();
	if( nModel >0)
		nComp = network[0].compts.size();

	for(vector<PN2SModel>::iterator n = network.begin(); n != network.end(); ++n) {
		res = importCompts(n->compts);
		assert(res==NO_ERROR);
		res = importHHChannels(n->hhChannels);
		assert(res==NO_ERROR);
	}
	return NO_ERROR;
}

hscError PN2S_NetworkAnalyzer::importCompts(vector<PN2SModel_Compartment> &cmpts)
{
	for(vector<PN2SModel_Compartment>::iterator n = cmpts.begin(); n != cmpts.end(); ++n) {
		allCompartments.push_back(n.base());
		importHHChannels(n->hhchannels);
	}
	return NO_ERROR;
}

hscError PN2S_NetworkAnalyzer::importHHChannels(vector<PN2SModel_HHChannel> &chs)
{
	if(chs.size() > 0)
	{
		for(vector<PN2SModel_HHChannel>::iterator n = chs.begin(); n != chs.end(); ++n) {
			allHHChannels.push_back(n.base());
		}
	}
	return NO_ERROR;
}
