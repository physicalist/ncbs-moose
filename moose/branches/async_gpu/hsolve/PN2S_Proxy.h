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
	int a;
	static void Setup(double dt);
	static void CreateCompartmentModel(Eref hsolve, Id seed);
	static void Reinit();
	static void Process(ProcPtr info);

	//Setter and Getter functions
	enum FIELD {CM_FIELD, EM_FIELD, RM_FIELD, RA_FIELD,INIT_VM_FIELD, VM_FIELD, INJECT_FIELD};

	static void setValue( Id id, TYPE_ value , FIELD n);
	static TYPE_ getValue( Id id , FIELD n);

private:
	static void walkTree( Id seed, vector<Id> &compartmentIds );
	static void storeTree(vector<Id> &compartmentIds, vector< TreeNodeStruct >& tree);
};
