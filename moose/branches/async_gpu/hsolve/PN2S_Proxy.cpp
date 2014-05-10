///////////////////////////////////////////////////////////
//  PN2S_Proxy.cpp
//  Implementation of the Class PN2S_Proxy
//  Created on:      26-Dec-2013 4:08:07 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_Proxy.h"
#include "PN2S/Definitions.h"
#include "PN2S/model/PN2SModel.h"
#include "PN2S/PN2S_Manager.h"
#include "HSolveUtils.h"
// #include "PN2S/modelr/PN2SModel_Compartment.h"
#include "../biophysics/Compartment.h" //For get info from Shell
/**
 * This method is responsible to create model and get pertinent information from
 * the Shell and send it to Manager
 */

void PN2S_Proxy::InsertCompartmentModel(Id seed, double dt){

	//Get Compartment id's with hine's index order
	vector<Id> compartmentIds;
	walkTree(seed,compartmentIds);

	int nCompt = compartmentIds.size();

	PN2SModel neutral(seed.value());

	/**
	 * Create Compartmental Model
	 */

	// A map from the MOOSE Id to Hines' index.
	map< Id, unsigned int > hinesIndex;
	for ( unsigned int i = 0; i < nCompt; ++i )
		hinesIndex[ compartmentIds[ i ] ] = i;

	vector< Id > childId;
	vector< Id >::iterator child;

	neutral.compts.resize(nCompt);
	for (uint i = 0; i < nCompt; ++i) {
		neutral.compts[i].Ra = HSolveUtils::get< Compartment, double >( compartmentIds[ i ], "Ra" );
		neutral.compts[i].Rm = HSolveUtils::get< Compartment, double >( compartmentIds[ i ], "Rm" );
		neutral.compts[i].Cm = HSolveUtils::get< Compartment, double >( compartmentIds[ i ], "Cm" );
		neutral.compts[i].Em = HSolveUtils::get< Compartment, double >( compartmentIds[ i ], "Em" );
		neutral.compts[i].initVm = HSolveUtils::get< Compartment, double >( compartmentIds[ i ], "initVm" );

		//Find Children
		childId.clear();
		HSolveUtils::children( compartmentIds[ i ], childId );
		for ( child = childId.begin(); child != childId.end(); ++child )
			neutral.compts[i].children.push_back( hinesIndex[ *child ] );

//		_printVector(tree[i].children.size(), &(tree[i].children[0]));
	}


	//Now model is ready to import into the PN2S
	PN2S_Manager::InsertModel(neutral);
}

void PN2S_Proxy::walkTree( Id seed, vector<Id> &compartmentIds )
{
    //~ // Dirty call to explicitly call the compartments reinitFunc.
    //~ // Should be removed eventually, and replaced with a cleaner way to
    //~ // initialize the model being read.
    //~ HSolveUtils::initialize( seed );

    // Find leaf node
    Id previous;
    vector< Id > adjacent;
    HSolveUtils::adjacent( seed, adjacent );
    if ( adjacent.size() > 1 )
        while ( !adjacent.empty() )
        {
            previous = seed;
            seed = adjacent[ 0 ];

            adjacent.clear();
            HSolveUtils::adjacent( seed, previous, adjacent );
        }

    // Depth-first search
    vector< vector< Id > > cstack;
    Id above;
    Id current;
    cstack.resize( 1 );
    cstack[ 0 ].push_back( seed );
    while ( !cstack.empty() )
    {
        vector< Id >& top = cstack.back();

        if ( top.empty() )
        {
            cstack.pop_back();
            if ( !cstack.empty() )
                cstack.back().pop_back();
        }
        else
        {
            if ( cstack.size() > 1 )
                above = cstack[ cstack.size() - 2 ].back();

            current = top.back();
            compartmentIds.push_back( current );

            cstack.resize( cstack.size() + 1 );
            HSolveUtils::adjacent( current, above, cstack.back() );
        }
    }

    // Compartments get ordered according to their hines' indices once this
    // list is reversed.
    reverse( compartmentIds.begin(), compartmentIds.end() );
}


void PN2S_Proxy::Reinit(){
	PN2S_Manager::Reinit();
}


/**
 * If it's the first time to execute, prepare solver
 */
void PN2S_Proxy::Process(int id){

}
