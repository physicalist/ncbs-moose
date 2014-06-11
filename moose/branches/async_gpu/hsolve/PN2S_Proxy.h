///////////////////////////////////////////////////////////
//  PN2S_Proxy.h
//  Implementation of the Class PN2S_Proxy
//  Created on:      26-Dec-2013 4:08:07 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#pragma once

/**
 * The class that use PN2S package and deal with moose constrains.
 */
#include "PN2S/headers.h"
#include "header.h" //Moose parts
#include "HinesMatrix.h"


class PN2S_Proxy
{

public:
	static void Setup(double dt);
	static void CreateCompartmentModel(Id seed);
	static void Reinit(Eref hsolve);
	static void Process(ProcPtr info);

	static void setValue( Id , TYPE_, pn2s::FIELD::TYPE);
	static TYPE_ getValue( Id , pn2s::FIELD::TYPE);

private:
	static void walkTree( Id seed, vector<Id> &compartmentIds );
	static void storeTree(vector<Id> &compartmentIds, vector< TreeNodeStruct >& tree);
};
