///////////////////////////////////////////////////////////
//  PN2S_Proxy.cpp
//  Implementation of the Class PN2S_Proxy
//  Created on:      26-Dec-2013 4:08:07 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_Proxy.h"
#include "PN2S/headers.h"
#include "PN2S/core/models/NeuronVector.h"
#include "PN2S/core/Network.h"
#include "PN2S/Manager.h"
#include "PN2S/core/models/SolverComps.h"
#include "HSolveUtils.h"

//For get info from Shell
#include "../headers.h"
#include "../biophysics/Compartment.h"
#include "ZombieCompartment.h"
#include "../biophysics/CaConc.h"
#include "ZombieCaConc.h"
#include "../biophysics/HHGate.h"
#include "../biophysics/ChanBase.h"
#include "../biophysics/HHChannel.h"
#include "ZombieHHChannel.h"
#include "../shell/Wildcard.h"
#include "../shell/Shell.h"

//Static objects
static map< uint, Id > _objectMap;

using namespace pn2s;



models::NeuronVector neurons;

//Getter and Setter
//TYPE_ _getValue(uint id, SolverComps::Fields field)
//{
//	switch(field)
//	{
//		case SolverComps::CM_FIELD:
//			return ::Field< double >::get( _objectMap[ id ], "Cm" );
//		case SolverComps::EM_FIELD:
//			return ::Field< double >::get( _objectMap[ id ], "Em" );
//		case SolverComps::RM_FIELD:
//			return ::Field< double >::get( _objectMap[ id ], "Rm" );
//		case SolverComps::RA_FIELD:
//			return ::Field< double >::get( _objectMap[ id ], "Ra" );
//		case SolverComps::VM_FIELD:
//			return ::Field< double >::get( _objectMap[ id ], "Vm" );
//		case SolverComps::INIT_VM_FIELD:
//			return ::Field< double >::get( _objectMap[ id ], "initVm" );
//	}
//	return 0;
//}

/**
 * This method is responsible to create model and get pertinent information from
 * the Shell and send it to Manager
 */

void PN2S_Proxy::Setup(double dt)
{
	cout << "Size: " << sizeof(PN2S_Proxy) << flush;
	Manager::Setup(dt);
	_objectMap.clear();

	//Register Setter and Getter
//	SolverComps::Fetch_Func = &_getValue;
}


void PN2S_Proxy::CreateCompartmentModel(Eref hsolve, Id seed){

	//Get Compartment id's with hine's index order
	vector<Id> compartmentIds;
	walkTree(seed,compartmentIds);

	int nCompt = compartmentIds.size();

	// A map from the MOOSE Id to Hines' index. It will remove after this method
	map< Id, unsigned int > hinesIndex;
	for ( int i = 0; i < nCompt; ++i )
	{
		hinesIndex[ compartmentIds[ i ] ] = i;
		_objectMap[compartmentIds[ i ].value()] = compartmentIds[ i ];
	}

	/**
	 * Create Compartmental Model
	 */
	vector< Id > childId;
	vector< Id >::iterator child;

	models::Neuron* n = neurons.Create(seed.value());

	for (int i = 0; i < nCompt; ++i) {
		//Assign a general ID to each compartment
		models::Compartment* c = n->compt().Create(compartmentIds[ i ].value());

		//Find Children
		childId.clear();
		HSolveUtils::children( compartmentIds[ i ], childId );
		for ( child = childId.begin(); child != childId.end(); ++child )
		{
			c->children.push_back( hinesIndex[ *child ] );
		}
//		_printVector(tree[i].children.size(), &(tree[i].children[0]));
	}


	/**
	 * Zumbify
	 */
    vector< Id >::const_iterator i;
	vector< ObjId > temp;

    for ( i = compartmentIds.begin(); i != compartmentIds.end(); ++i )
		temp.push_back( ObjId( *i, 0 ) );
	Shell::dropClockMsgs( temp, "init" );
	Shell::dropClockMsgs( temp, "process" );
    for ( i = compartmentIds.begin(); i != compartmentIds.end(); ++i )
        CompartmentBase::zombify( i->eref().element(),
					   ZombieCompartment::initCinfo(), hsolve.id() );

//	temp.clear();
//    for ( i = caConcId_.begin(); i != caConcId_.end(); ++i )
//		temp.push_back( ObjId( *i, 0 ) );
//	Shell::dropClockMsgs( temp, "process" );
//    for ( i = caConcId_.begin(); i != caConcId_.end(); ++i )
//        ZombieCaConc::zombify( hsolve.element(), i->eref().element() );
//
//	temp.clear();
//    for ( i = channelId_.begin(); i != channelId_.end(); ++i )
//		temp.push_back( ObjId( *i, 0 ) );
//	Shell::dropClockMsgs( temp, "process" );
//    for ( i = channelId_.begin(); i != channelId_.end(); ++i )
//        ZombieHHChannel::zombify( hsolve.element(), i->eref().element() );

	/**
	 * Now model is ready to import into the PN2S
	 */
//	Manager::InsertModel(neutral);
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
    for (int var = 0; var < compartmentIds.size(); ++var) {
        	    	   	cout << compartmentIds[var] << " " << flush;
        	    	}
    // Compartments get ordered according to their hines' indices once this
    // list is reversed.
    reverse( compartmentIds.begin(), compartmentIds.end() );
}


void PN2S_Proxy::Reinit(){
	Manager::Reinit();
}

/**
 * If it's the first time to execute, prepare solver
 */
void PN2S_Proxy::Process(ProcPtr info){
//	Manager::Process();
}

/**
 * Interface Set/Get functions
 */
void PN2S_Proxy::setValue( Id id, TYPE_ value , FIELD n)
{
//	switch(n)
//	{
//		case FIELD::CM_FIELD:
//			return ::Field< double >::get( _objects[ id ], "Cm" );
//	}

//	cout << id << ".Cm = "<< __func__<<value<<flush;
}

TYPE_ PN2S_Proxy::getValue( Id id, FIELD n)
{
//    assert(this);
//    unsigned int index = localIndex( id );
//    assert( index < V_.size() );
//    return V_[ index ];
	return 11;
}
