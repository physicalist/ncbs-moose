/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "RateTerm.h"
#include "SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "Stoich.h"
#include "ZombieReac.h"
#include "Reac.h"
#include "ElementValueFinfo.h"
#include "DataHandlerWrapper.h"

static SrcFinfo2< double, double > toSub( 
		"toSub", 
		"Sends out increment of molecules on product each timestep"
	);
static SrcFinfo2< double, double > toPrd( 
		"toPrd", 
		"Sends out increment of molecules on product each timestep"
	);

static DestFinfo sub( "subDest",
		"Handles # of molecules of substrate",
		new OpFunc1< ZombieReac, double >( &ZombieReac::sub ) );

static DestFinfo prd( "prdDest",
		"Handles # of molecules of product",
		new OpFunc1< ZombieReac, double >( &ZombieReac::prd ) );
	
static Finfo* subShared[] = {
	&toSub, &sub
};

static Finfo* prdShared[] = {
	&toPrd, &prd
};

const Cinfo* ZombieReac::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ElementValueFinfo< ZombieReac, double > kf(
			"kf",
			"Forward rate constant",
			&ZombieReac::setKf,
			&ZombieReac::getKf
		);

		static ElementValueFinfo< ZombieReac, double > kb(
			"kb",
			"Backward rate constant",
			&ZombieReac::setKb,
			&ZombieReac::getKb
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< ZombieReac >( &ZombieReac::process ) );

		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< ZombieReac >( &ZombieReac::reinit ) );

		static DestFinfo group( "group",
			"Handle for grouping. Doesn't do anything.",
			new OpFuncDummy() );

		//////////////////////////////////////////////////////////////
		// SharedMsg Definitions
		//////////////////////////////////////////////////////////////
		static SharedFinfo sub( "sub",
			"Connects to substrate molecule",
			subShared, sizeof( subShared ) / sizeof( const Finfo* )
		);
		static SharedFinfo prd( "prd",
			"Connects to substrate molecule",
			prdShared, sizeof( prdShared ) / sizeof( const Finfo* )
		);
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);

	static Finfo* zombieReacFinfos[] = {
		&kf,		// Value
		&kb,		// Value
		&sub,		// SharedFinfo
		&prd,		// SharedFinfo
		&proc,		// SharedFinfo
	};

	static Cinfo zombieReacCinfo (
		"ZombieReac",
		Neutral::initCinfo(),
		zombieReacFinfos,
		sizeof( zombieReacFinfos ) / sizeof ( Finfo* ),
		new Dinfo< ZombieReac >()
	);

	return &zombieReacCinfo;
}

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* zombieReacCinfo = ZombieReac::initCinfo();

ZombieReac::ZombieReac()
{;}

ZombieReac::~ZombieReac()
{;}


//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

// Doesn't do anything on its own.
void ZombieReac::process( const Eref& e, ProcPtr p )
{;}

void ZombieReac::reinit( const Eref& e, ProcPtr p )
{;}


void ZombieReac::sub( double v )
{
}

void ZombieReac::prd( double v )
{
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void ZombieReac::setKf( Eref e, const Qinfo* q, double v )
{
	rates_[ convertIdToReacIndex( e.id() ) ]->setR1( v );
}

double ZombieReac::getKf( Eref e, const Qinfo* q ) const
{
	return rates_[ convertIdToReacIndex( e.id() ) ]->getR1();
}

void ZombieReac::setKb( Eref e, const Qinfo* q, double v )
{
	rates_[ convertIdToReacIndex( e.id() ) ]->setR2( v );
}

double ZombieReac::getKb( Eref e, const Qinfo* q ) const
{
	return rates_[ convertIdToReacIndex( e.id() ) ]->getR2();
}

//////////////////////////////////////////////////////////////
// Utility function
//////////////////////////////////////////////////////////////

ZeroOrder* ZombieReac::makeHalfReaction( 
	Element* orig, double rate, const SrcFinfo* finfo ) const
{
	vector< Id > mols;
	unsigned int numReactants = orig->getOutputs( mols, finfo ); 
	ZeroOrder* rateTerm;
	if ( numReactants == 1 ) {
		rateTerm = 
			new FirstOrder( rate, &S_[ convertIdToMolIndex( mols[0] ) ] );
	} else if ( numReactants == 2 ) {
		rateTerm = new SecondOrder( rate,
				&S_[ convertIdToMolIndex( mols[0] ) ], 
				&S_[ convertIdToMolIndex( mols[1] ) ] );
	} else if ( numReactants > 2 ) {
		vector< const double* > v;
		for ( unsigned int i = 0; i < numReactants; ++i ) {
			v.push_back( &S_[ convertIdToMolIndex( mols[i] ) ] );
		}
		rateTerm = new NOrder( rate, v );
	} else {
		cout << "Error: ZombieReac::makeHalfReaction: zero reactants\n";
	}
	return rateTerm;
}

// static func
void ZombieReac::zombify( Element* solver, Element* orig )
{
	static const SrcFinfo* sub = dynamic_cast< const SrcFinfo* >(
		Reac::initCinfo()->findFinfo( "toSub" ) );
	static const SrcFinfo* prd = dynamic_cast< const SrcFinfo* >(
		Reac::initCinfo()->findFinfo( "toPrd" ) );
	
	assert( sub );
	assert( prd );

	Element temp( orig->id(), zombieReacCinfo, solver->dataHandler() );
	Eref zer( &temp, 0 );
	Eref oer( orig, 0 );

	ZombieReac* z = reinterpret_cast< ZombieReac* >( zer.data() );
	Reac* reac = reinterpret_cast< Reac* >( oer.data() );

	ZeroOrder* forward = z->makeHalfReaction( orig, reac->getKf(), sub );
	ZeroOrder* reverse = z->makeHalfReaction( orig, reac->getKb(), prd );

	unsigned int rateIndex = z->convertIdToReacIndex( orig->id() );
	z->rates_[ rateIndex ] = new BidirectionalReaction( forward, reverse );

	vector< unsigned int > molIndex;
	unsigned int numReactants = forward->getReactants( molIndex, z->S_ );
	for ( unsigned int i = 0; i < numReactants; ++i ) {
		int temp = z->N_.get( molIndex[i], rateIndex );
		z->N_.set( molIndex[i], rateIndex, temp - 1 );
	}

	numReactants = reverse->getReactants( molIndex, z->S_ );
	for ( unsigned int i = 0; i < numReactants; ++i ) {
		int temp = z->N_.get( molIndex[i], rateIndex );
		z->N_.set( molIndex[i], rateIndex, temp + 1 );
	}

	DataHandler* dh = new DataHandlerWrapper( solver->dataHandler() );
	orig->zombieSwap( zombieReacCinfo, dh );
}

// Static func
void ZombieReac::unzombify( Element* zombie )
{
	Element temp( zombie->id(), zombie->cinfo(), zombie->dataHandler() );
	Eref zer( &temp, 0 );
	Eref oer( zombie, 0 );

	ZombieReac* z = reinterpret_cast< ZombieReac* >( zer.data() );

	// Here I am unsure how to recreate the correct kind of data handler
	// for the original. Do later.
	DataHandler* dh = 0;

	zombie->zombieSwap( Reac::initCinfo(), dh );

	Reac* m = reinterpret_cast< Reac* >( oer.data() );

	m->setKf( z->getKf( zer, 0 ) );
	m->setKb( z->getKb( zer, 0 ) );
}
