///////////////////////////////////////////////////////////
//  PN2S_Proxy.h
//  Implementation of the Class PN2S_Proxy
//  Created on:      26-Dec-2013 4:08:07 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(EA_27150678_5056_4754_82F6_A77DCEB1BC1C__INCLUDED_)
#define EA_27150678_5056_4754_82F6_A77DCEB1BC1C__INCLUDED_

/**
 * The class that use PN2S package and deal with moose constrains.
 */
#include "PN2S/PN2S.h"
#include "header.h" //Moose parts
#include "HinesMatrix.h"


class PN2S_Proxy
{

public:
	static void Setup(double dt);
	static void InsertCompartmentModel(Eref hsolve, Id seed);
	static void Reinit();
	static void Process(ProcPtr info);
private:
	static void walkTree( Id seed, vector<Id> &compartmentIds );
	static void storeTree(vector<Id> &compartmentIds, vector< TreeNodeStruct >& tree);

	static void zombify( Element* solver, Element* orig);
};
#endif // !defined(EA_27150678_5056_4754_82F6_A77DCEB1BC1C__INCLUDED_)
