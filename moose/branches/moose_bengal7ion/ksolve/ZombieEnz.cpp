/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#include "header.h"

#include "lookupVolumeFromMesh.h"
#include "RateTerm.h"
#include "FuncTerm.h"
#include "SparseMatrix.h"
#include "KinSparseMatrix.h"
#include "Stoich.h"

#include "EnzBase.h"
#include "CplxEnzBase.h"
#include "ZombieEnz.h"

const Cinfo* ZombieEnz::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// Shared Msg Definitions
		//////////////////////////////////////////////////////////////

	static Dinfo< ZombieEnz > dinfo;
	static Cinfo zombieEnzCinfo (
		"ZombieEnz",
		CplxEnzBase::initCinfo(),
		0,
		0,
		&dinfo
	);

	return &zombieEnzCinfo;
}
//////////////////////////////////////////////////////////////

static const Cinfo* zombieEnzCinfo = ZombieEnz::initCinfo();

static const SrcFinfo2< double, double >* subOut =
	dynamic_cast< const SrcFinfo2< double, double >* >(
	zombieEnzCinfo->findFinfo( "subOut" ) );

//////////////////////////////////////////////////////////////
// ZombieEnz internal functions
//////////////////////////////////////////////////////////////

ZombieEnz::ZombieEnz( )
		: 
				stoich_( 0 ),
				concK1_( 1.0 )
{ ; }

ZombieEnz::~ZombieEnz( )
{ ; }

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

/*
void ZombieEnz::vRemesh( const Eref& e )
{   
	stoich_->setEnzK1( e, concK1_ );
}   
*/


//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

// v is in number units.
void ZombieEnz::vSetK1( const Eref& e, double v )
{
	double volScale = 
		convertConcToNumRateUsingMesh( e, subOut, 1 );

	concK1_ = v / volScale;
	stoich_->setEnzK1( e, concK1_ );
}

// v is In number units.
double ZombieEnz::vGetK1( const Eref& e ) const
{
	return stoich_->getEnzNumK1( e );
}

void ZombieEnz::vSetK2( const Eref& e, double v )
{
	stoich_->setEnzK2( e, v );
}

double ZombieEnz::vGetK2( const Eref& e ) const
{
	return stoich_->getEnzK2( e );
}

void ZombieEnz::vSetKcat( const Eref& e, double v )
{
	stoich_->setEnzK3( e, v );
}

double ZombieEnz::vGetKcat( const Eref& e ) const
{
	return stoich_->getEnzK3( e );
}


void ZombieEnz::vSetKm( const Eref& e, double v )
{
	double k2 = getK2( e );
	double k3 = getKcat( e );
	concK1_ = ( k2 + k3 ) / v;
	stoich_->setEnzK1( e, concK1_ );
}

double ZombieEnz::vGetKm( const Eref& e ) const
{
	double k2 = getK2( e );
	double k3 = getKcat( e );

	return ( k2 + k3 ) / concK1_;
}

void ZombieEnz::vSetNumKm( const Eref& e, double v )
{
	double k2 = getK2( e );
	double k3 = getKcat( e );
	double volScale = 
		convertConcToNumRateUsingMesh( e, subOut, 1 );
	concK1_ = volScale * ( k2 + k3 ) / v;

	stoich_->setEnzK1( e, concK1_ );
}

double ZombieEnz::vGetNumKm( const Eref& e ) const
{
	double k2 = vGetK2( e );
	double k3 = vGetKcat( e );
	double volScale = 
		convertConcToNumRateUsingMesh( e, subOut, 1 );

	return ( k2 + k3 ) / ( concK1_ * volScale );
}

void ZombieEnz::vSetRatio( const Eref& e, double v )
{
	double Km = getKm( e );
	double k2 = getK2( e );
	double k3 = getKcat( e );

	k2 = v * k3;

	stoich_->setEnzK2( e, k2 );
	double k1 = ( k2 + k3 ) / Km;

	setConcK1( e, k1 );
}

double ZombieEnz::vGetRatio( const Eref& e ) const
{
	double k2 = getK2( e );
	double k3 = getKcat( e );
	return k2 / k3;
}

void ZombieEnz::vSetConcK1( const Eref& e, double v )
{
	concK1_ = v;
	stoich_->setEnzK1( e, v );
}

double ZombieEnz::vGetConcK1( const Eref& e ) const
{
	return concK1_;
}

//////////////////////////////////////////////////////////////
// Utility function
//////////////////////////////////////////////////////////////

// static func
void ZombieEnz::setSolver( Id stoich, Id enz )
{
	static const Finfo* subFinfo = Cinfo::find("Enz")->findFinfo( "subOut");
	static const Finfo* prdFinfo = Cinfo::find("Enz")->findFinfo( "prdOut");
	static const Finfo* enzFinfo = Cinfo::find("Enz")->findFinfo( "enzOut");
	static const Finfo* cplxFinfo= Cinfo::find("Enz")->findFinfo("cplxOut");

	assert( subFinfo );
	assert( prdFinfo );
	assert( enzFinfo );
	assert( cplxFinfo );
	vector< Id > temp;
	unsigned int numReactants;
	numReactants = enz.element()->getNeighbours( temp, enzFinfo ); 
	assert( numReactants == 1 );
	Id enzMol = temp[0];
	vector< Id > subs;
	numReactants = enz.element()->getNeighbours( subs, subFinfo ); 
	assert( numReactants > 0 );
	numReactants = enz.element()->getNeighbours( temp, cplxFinfo ); 
	assert( numReactants == 1 );
	Id cplx = temp[0];
	vector< Id > prds;
	numReactants = enz.element()->getNeighbours( prds, prdFinfo ); 
	assert( numReactants > 0 );

	assert( stoich.element()->cinfo()->isA( "Stoich" ) );
	stoich_ = reinterpret_cast< Stoich* >( stoich.eref().data() );
	stoich_->installEnzyme( enz, enzMol, cplx, subs, prds );
}
