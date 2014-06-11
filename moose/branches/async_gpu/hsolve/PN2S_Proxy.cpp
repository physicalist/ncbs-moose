///////////////////////////////////////////////////////////
//  PN2S_Proxy.cpp
//  Implementation of the Class PN2S_Proxy
//  Created on:      26-Dec-2013 4:08:07 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#include "PN2S_Proxy.h"
#include "PN2S/headers.h"
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

using namespace pn2s;

//TODO: Replace with hash maps
map< int, Location > _compartmentMap;
//map< uint, Id > _idMap;
vector<Id> _all_compartmentIds;
/**
 * This method is responsible to create model and get pertinent information from
 * the Shell and send it to Manager
 */

void PN2S_Proxy::Setup(double dt)
{
	Manager::Setup(dt);
	_compartmentMap.clear();
//	_idMap.clear();
	_all_compartmentIds.clear();
}

/**
 * Create Compartmental Model
 */
void PN2S_Proxy::CreateCompartmentModel(Id seed){

	//Get Compartment id's with hine's index order
	vector<Id> compartmentIds;
	walkTree(seed,compartmentIds);
	int nCompt = compartmentIds.size();

	//TODO: Merge it with _all_compartmentIds
	_all_compartmentIds.insert( _all_compartmentIds.end(), compartmentIds.begin(), compartmentIds.end() );

	models::Model neutral(seed.value());


	// A map from the MOOSE Id to Hines' index.
	map< Id, unsigned int > hinesIndex;
	for ( int i = 0; i < nCompt; ++i )
	{
		hinesIndex[ compartmentIds[ i ] ] = i; //TODO: go to below loop
//		_idMap[compartmentIds[ i ].value()] = compartmentIds[ i ];
	}

	/**
	 * Create Compartmental Model
	 */
	vector< Id > childId;
	vector< Id >::iterator child;

	for (int i = 0; i < nCompt; ++i) {
		//Assign a general ID to each compartment
		models::Compartment c(compartmentIds[ i ].value());

		//Find Children
		childId.clear();
		HSolveUtils::children( compartmentIds[ i ], childId );
		for ( child = childId.begin(); child != childId.end(); ++child )
		{
			c.children.push_back( hinesIndex[ *child ] );
		}
		neutral.compts.push_back(c);
	}

	/**
	 * Now model is ready to import into the PN2S
	 */
    Manager::InsertModelShape(neutral);
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


void PN2S_Proxy::Reinit(Eref hsolve){
	//Create model structures and Allocate memory
	Manager::Allocate();

	//create a map from Id to Location
	typename vector<Device>::iterator dev;
	typename vector<ModelPack>::iterator mp;
	for( dev = DeviceManager::_device.begin(); dev != DeviceManager::_device.end(); ++dev) {
		for( mp = dev->_modelPacks.begin(); mp != dev->_modelPacks.end(); ++mp) {
			for (size_t m = 0; m < mp->stat.nModels; ++m) {
				models::Model& model = mp->models[m];
				for (size_t c = 0; c < mp->stat.nCompts; ++c) {
					models::Compartment* cmp = &(model.compts[c]);
					_compartmentMap[cmp->gid] = cmp->location;
				}
			}
		}
	}

	/**
	 * Zumbify and Copy data values
	 */
    vector< Id >::const_iterator i;
	vector< ObjId > temp;

    for ( i = _all_compartmentIds.begin(); i != _all_compartmentIds.end(); ++i )
		temp.push_back( ObjId( *i, 0 ) );
	Shell::dropClockMsgs( temp, "init" );
	Shell::dropClockMsgs( temp, "process" );
    for ( i = _all_compartmentIds.begin(); i != _all_compartmentIds.end(); ++i )
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

    //Prepare solvers
	Manager::PrepareSolvers();
}

/**
 * If it's the first time to execute, prepare solver
 */
void PN2S_Proxy::Process(ProcPtr info){
	Manager::Process();
}

/**
 * Interface Set/Get functions
 */
void PN2S_Proxy::setValue( Id id, TYPE_ value , FIELD::TYPE n)
{
	Location l = _compartmentMap[id.value()];
	DeviceManager::_device[0]._modelPacks[l.address]._compsSolver.SetValue(l.index,n,value);
}

TYPE_ PN2S_Proxy::getValue( Id id, FIELD::TYPE n)
{
//    models::Compartment* c = _compartmentMap[id.value()];
//    return DeviceManager::_device[0]._modelPacks[0]._compsSolver.GetValue(c,n);
	return 11;
}
