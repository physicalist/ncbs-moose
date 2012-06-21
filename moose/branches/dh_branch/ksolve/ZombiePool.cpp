/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "StoichHeaders.h"
#include "ZombiePool.h"
// #include "Pool.h"
#include "lookupSizeFromMesh.h"
#include "ElementValueFinfo.h"
#include "ZombieHandler.h"

#define EPSILON 1e-15

const Cinfo* ZombiePool::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions: All inherited from PoolBase
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// MsgDest Definitions: All inherited from PoolBase
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// SrcFinfo Definitions: All inherited from PoolBase
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// SharedMsg Definitions: All inherited from PoolBase
		//////////////////////////////////////////////////////////////

	// Note that here the isOneZombie_ flag on the Dinfo constructor is
	// true. This means that the duplicate and copy operations only make
	// one copy, regardless of how many dims they are requested to do.
	static Cinfo zombiePoolCinfo (
		"ZombiePool",
		PoolBase::initCinfo(),
		0,
		0,
		new Dinfo< ZombiePool >( true )
	);

	return &zombiePoolCinfo;
}




//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* zombiePoolCinfo = ZombiePool::initCinfo();

static const SrcFinfo1< double >* requestSize = 
    dynamic_cast< const SrcFinfo1< double >* >(  
    zombiePoolCinfo->findFinfo( "requestSize" ) );

ZombiePool::ZombiePool()
{;}

ZombiePool::~ZombiePool()
{;}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

// vRemesh: All the work is done by the message from the compartment to the
// Stoich. None of the ZombiePools is remeshed directly. However, their
// DataHandlers need updating.
void ZombiePool::vRemesh( const Eref& e, const Qinfo* q, 
	unsigned int numTotalEntries, unsigned int startEntry, 
	const vector< unsigned int >& localIndices, 
	const vector< double >& vols )
{
	;
	if ( e.index().value() != 0 )
		return;
	Neutral* n = reinterpret_cast< Neutral* >( e.data() );
	if ( vols.size() != e.element()->dataHandler()->localEntries() )
		n->setLastDimension( e, q, vols.size() );

	/*
	// We only want to update vols and concs once. The updateMeshVols 
	// call does this for all pools in the stoich.
	unsigned int poolIndex = stoich_->convertIdToPoolIndex( e.id() );
	if ( poolIndex == 0 )
		stoich_->updateMeshVols( vols );
	*/
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void ZombiePool::vSetN( const Eref& e, const Qinfo* q, double v )
{
	stoich_->innerSetN( e.index().value(), e.id(), v );
	// S_[ e.index().value() ][ convertIdToPoolIndex( e.id() ) ] = v;
}

double ZombiePool::vGetN( const Eref& e, const Qinfo* q ) const
{
	return stoich_->S( e.index().value() )
		[ stoich_->convertIdToPoolIndex( e.id() ) ];
}

void ZombiePool::vSetNinit( const Eref& e, const Qinfo* q, double v )
{
	stoich_->innerSetNinit( e.index().value(), e.id(), v );
	// Sinit_[ e.index().value() ][ convertIdToPoolIndex( e.id() ) ] = v;
}

double ZombiePool::vGetNinit( const Eref& e, const Qinfo* q ) const
{
	return stoich_->Sinit( e.index().value() )
		[ stoich_->convertIdToPoolIndex( e.id() ) ];
}

void ZombiePool::vSetConc( const Eref& e, const Qinfo* q, double conc )
{
	// unsigned int pool = convertIdToPoolIndex( e.id() );
	double n = NA * conc * lookupSizeFromMesh( e, requestSize );
	stoich_->innerSetN( e.index().value(), e.id(), n );
}

double ZombiePool::vGetConc( const Eref& e, const Qinfo* q ) const
{
	unsigned int pool = stoich_->convertIdToPoolIndex( e.id() );
	return stoich_->S( e.index().value() )[ pool ] / 
		( NA * lookupSizeFromMesh( e, requestSize ) );
}

void ZombiePool::vSetConcInit( const Eref& e, const Qinfo* q, double conc )
{
	double n = NA * conc * lookupSizeFromMesh( e, requestSize );
	stoich_->innerSetNinit( e.index().value(), e.id(), n );
}

double ZombiePool::vGetConcInit( const Eref& e, const Qinfo* q ) const
{
	unsigned int pool = stoich_->convertIdToPoolIndex( e.id() );
	return stoich_->Sinit( e.index().value() )[ pool ] / ( NA * lookupSizeFromMesh( e, requestSize ) );
}

void ZombiePool::vSetDiffConst( const Eref& e, const Qinfo* q, double v )
{
	stoich_->setDiffConst( stoich_->convertIdToPoolIndex( e.id() ), v );
}

double ZombiePool::vGetDiffConst( const Eref& e, const Qinfo* q ) const
{
	return stoich_->getDiffConst( stoich_->convertIdToPoolIndex( e.id() ) );
}

void ZombiePool::vSetSize( const Eref& e, const Qinfo* q, double v )
{
	// Illegal operation.
}

double ZombiePool::vGetSize( const Eref& e, const Qinfo* q ) const
{
	return lookupSizeFromMesh( e, requestSize );
}

void ZombiePool::vSetSpecies( const Eref& e, const Qinfo* q, unsigned int v )
{
	stoich_->setSpecies( stoich_->convertIdToPoolIndex( e.id() ), v );
}

unsigned int ZombiePool::vGetSpecies( const Eref& e, const Qinfo* q ) const
{
	return stoich_->getSpecies( stoich_->convertIdToPoolIndex( e.id() ) );
}

/*
void ZombiePool::vHandleMolWt( const Eref& e, const Qinfo* q, double v )
{
	;
}
*/
//////////////////////////////////////////////////////////////
// Zombie conversion functions.
//////////////////////////////////////////////////////////////

// static func
void ZombiePool::zombify( Element* solver, Element* orig )
{
	////////////////////////////////////////////////////////
	// Get rid of Process message
	static const Finfo* procDest = 
		PoolBase::initCinfo()->findFinfo( "process");
	assert( procDest );

	const DestFinfo* df = dynamic_cast< const DestFinfo* >( procDest );
	assert( df );
	MsgId mid = orig->findCaller( df->getFid() );
	if ( mid != Msg::bad )
		Msg::deleteMsg( mid );
	////////////////////////////////////////////////////////

	DataHandler* origHandler = orig->dataHandler();
	DataHandler* dh = origHandler->copyUsingNewDinfo( 
		ZombiePool::initCinfo()->dinfo() );
	Element temp( orig->id(), zombiePoolCinfo, dh );
	Eref zombier( &temp, 0 );

	ZombiePool* z = reinterpret_cast< ZombiePool* >( zombier.data() );
	Eref oer( orig, 0 );

	PoolBase* m = reinterpret_cast< PoolBase* >( oer.data() );
	// May need to extend to entire array.
	z->stoich_ = reinterpret_cast< Stoich* >( solver->dataHandler()->data( 0 ) );
	z->stoich_->setConcInit( z->stoich_->convertIdToPoolIndex( orig->id() ),
		m->getConcInit( oer, 0 ) );
	z->vSetSpecies( zombier, 0, m->getSpecies( oer, 0 ) );
	z->vSetConcInit( zombier, 0, m->getConcInit( oer, 0 ) );
	z->vSetN( zombier, 0, m->getN( oer, 0 ) );
	z->vSetNinit( zombier, 0, m->getNinit( oer, 0 ) );
	z->vSetDiffConst( zombier, 0, m->getDiffConst( oer, 0 ) );
	// DataHandler* zh = new ZombieHandler( solver->dataHandler(), orig->dataHandler(), 0, numEntries );
	orig->zombieSwap( zombiePoolCinfo, dh );
	delete origHandler;
}

// Static func
void ZombiePool::unzombify( Element* zombie )
{
	Element temp( zombie->id(), zombie->cinfo(), zombie->dataHandler() );
	Eref zer( &temp, 0 );
	Eref oer( zombie, 0 );

	ZombiePool* z = reinterpret_cast< ZombiePool* >( zer.data() );

	// Here I am unsure how to recreate the correct kind of data handler
	// for the original. Do later.
	DataHandler* dh = 0;

	zombie->zombieSwap( PoolBase::initCinfo(), dh );

	PoolBase* m = reinterpret_cast< PoolBase* >( oer.data() );

	m->setN( oer, 0, z->getN( zer, 0 ) );
	m->setNinit( oer, 0, z->getNinit( zer, 0 ) );
	m->setSpecies( oer, 0, z->getSpecies( zer, 0 ) );
}
