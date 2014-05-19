///////////////////////////////////////////////////////////
//  PN2S_Proxy.cpp
//  Implementation of the Class PN2S_Proxy
//  Created on:      26-Dec-2013 4:08:07 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_Proxy.h"
#include "PN2S/PN2S.h"
#include "PN2S/core/models/PN2SModel.h"
#include "PN2S/PN2S_Manager.h"
#include "HSolveUtils.h"
// #include "PN2S/modelr/PN2SModel_Compartment.h"
#include "../biophysics/Compartment.h" //For get info from Shell

#include "PN2S/core/solvers/PN2S_SolverComps.h"

//Static objects
static map< uint, Id > _objects;

//Getter and Setter
CURRENT_TYPE _getValue(uint id, PN2S_SolverComps<CURRENT_TYPE,CURRENT_ARCH>::Fields field)
{
	switch(field)
	{
		case PN2S_SolverComps<CURRENT_TYPE,CURRENT_ARCH>::CM_FIELD:
			return HSolveUtils::get< Compartment, double >( _objects[ id ], "Cm" );
		case PN2S_SolverComps<CURRENT_TYPE,CURRENT_ARCH>::EM_FIELD:
			return HSolveUtils::get< Compartment, double >( _objects[ id ], "Em" );
		case PN2S_SolverComps<CURRENT_TYPE,CURRENT_ARCH>::RM_FIELD:
			return HSolveUtils::get< Compartment, double >( _objects[ id ], "Rm" );
		case PN2S_SolverComps<CURRENT_TYPE,CURRENT_ARCH>::RA_FIELD:
			return HSolveUtils::get< Compartment, double >( _objects[ id ], "Ra" );
		case PN2S_SolverComps<CURRENT_TYPE,CURRENT_ARCH>::VM_FIELD:
			return HSolveUtils::get< Compartment, double >( _objects[ id ], "Vm" );
		case PN2S_SolverComps<CURRENT_TYPE,CURRENT_ARCH>::INIT_VM_FIELD:
			return HSolveUtils::get< Compartment, double >( _objects[ id ], "initVm" );
	}
	return 0;
}

/**
 * This method is responsible to create model and get pertinent information from
 * the Shell and send it to Manager
 */

void PN2S_Proxy::Setup(double dt)
{
	PN2S_Manager::Setup(dt);
	_objects.clear();

	//Register Setter and Getter
	PN2S_SolverComps<CURRENT_TYPE,CURRENT_ARCH>::GetValue_Func = &_getValue;
}


void PN2S_Proxy::InsertCompartmentModel(Eref master_hsolve, Id seed){

	//Get Compartment id's with hine's index order
	vector<Id> compartmentIds;
	walkTree(seed,compartmentIds);

	int nCompt = compartmentIds.size();

	PN2SModel<CURRENT_TYPE,CURRENT_ARCH> neutral(seed.value());

	/**
	 * Create Compartmental Model
	 */

	// A map from the MOOSE Id to Hines' index.
	map< Id, unsigned int > hinesIndex;
	for ( int i = 0; i < nCompt; ++i )
	{
		hinesIndex[ compartmentIds[ i ] ] = i;
		_objects[compartmentIds[ i ].value()] = compartmentIds[ i ];
	}

	vector< Id > childId;
	vector< Id >::iterator child;

	neutral.compts.resize(nCompt);

	for (int i = 0; i < nCompt; ++i) {
		//Assign a general ID to each compartment
		neutral.compts[i].gid = compartmentIds[ i ].value();

		//Find Children
		childId.clear();
		HSolveUtils::children( compartmentIds[ i ], childId );
		for ( child = childId.begin(); child != childId.end(); ++child )
		{
			neutral.compts[i].children.push_back( hinesIndex[ *child ] );
		}

//		_printVector(tree[i].children.size(), &(tree[i].children[0]));
	}


	/**
	 * Zumbify
	 */
	vector< Id >::const_iterator i;
	for ( i = compartmentIds.begin(); i != compartmentIds.end(); ++i )
		zombify( master_hsolve.element(), i->eref().element() );

	/**
	 * Now model is ready to import into the PN2S
	 */
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

void PN2S_Proxy::zombify( Element* solver, Element* orig)
{
//    vector< Id >::const_iterator i;
//
//    for ( i = compartmentId_.begin(); i != compartmentId_.end(); ++i )
//        ZombieCompartment::zombify( hsolve.element(), i->eref().element() );
//
//    for ( i = caConcId_.begin(); i != caConcId_.end(); ++i )
//        ZombieCaConc::zombify( hsolve.element(), i->eref().element() );
//
//    for ( i = channelId_.begin(); i != channelId_.end(); ++i )
//        ZombieHHChannel::zombify( hsolve.element(), i->eref().element() );

	// Delete "process" msg.
	static const Finfo* procDest = Compartment::initCinfo()->findFinfo("process");
	assert( procDest );

	const DestFinfo* df = dynamic_cast< const DestFinfo* >( procDest );
	assert( df );
	ObjId mid = orig->findCaller( df->getFid() );
	if ( ! mid.bad() )
		Msg::deleteMsg( mid );

	//TODO: Check zumbswap is necessary or not

}

void PN2S_Proxy::Reinit(){
	PN2S_Manager::Reinit();
}


/**
 * If it's the first time to execute, prepare solver
 */
void PN2S_Proxy::Process(ProcPtr info){
	PN2S_Manager::Process();
}
