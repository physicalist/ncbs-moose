/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "StoichHeaders.h"
#include "ElementValueFinfo.h"
#include "PoolBase.h"
#include "Pool.h"
#include "BufPool.h"
#include "FuncPool.h"
#include "ReacBase.h"
#include "Reac.h"
#include "EnzBase.h"
#include "CplxEnzBase.h"
#include "Enz.h"
#include "MMenz.h"
#include "FuncTerm.h"
#include "SumTotalTerm.h"
#include "FuncBase.h"
#include "SumFunc.h"
#include "MathFunc.h"
#include "StoichPools.h"
#include "ZPool.h"
#include "ZBufPool.h"
#include "ZFuncPool.h"
#include "ZReac.h"
#include "ZEnz.h"
#include "ZMMenz.h"
#include "../shell/Shell.h"

#ifdef USE_GSL
#include <gsl/gsl_errno.h>
#endif

#define EPSILON 1e-15

const unsigned int StoichCore::PoolIsNotOnSolver = ~0;

const Cinfo* StoichCore::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ValueFinfo< StoichCore, bool > useOneWay(
			"useOneWayReacs",
			"Flag: use bidirectional or one-way reacs. One-way is needed"
			"for Gillespie type stochastic calculations. Two-way is"
			"likely to be margninally more efficient in ODE calculations",
			&StoichCore::setOneWay,
			&StoichCore::getOneWay
		);

		static ReadOnlyValueFinfo< StoichCore, unsigned int > nVarPools(
			"nVarPools",
			"Number of variable molecule pools in the reac system",
			&StoichCore::getNumVarPools
		);

		static ElementValueFinfo< StoichCore, string > path(
			"path",
			"Path of reaction system to take over",
			&StoichCore::setPath,
			&StoichCore::getPath
		);

		static ReadOnlyValueFinfo< StoichCore, double > estimatedDt(
			"estimatedDt",
			"Estimate of fastest (smallest) timescale in system."
			"This is fallible because it depends on instantaneous concs,"
			"which of course change over the course of the simulation.",
			&StoichCore::getEstimatedDt
		);

	static Finfo* stoichCoreFinfos[] = {
		&useOneWay,		// Value
		&nVarPools,		// Value
		&estimatedDt,		// ReadOnlyValue
		&path,			// Value
	};

	static Cinfo stoichCoreCinfo (
		"StoichCore",
		Neutral::initCinfo(),
		stoichCoreFinfos,
		sizeof( stoichCoreFinfos ) / sizeof ( Finfo* ),
		new Dinfo< StoichCore >()
	);

	return &stoichCoreCinfo;
}

//////////////////////////////////////////////////////////////
// Class definitions
//////////////////////////////////////////////////////////////
static const Cinfo* stoichCoreCinfo = StoichCore::initCinfo();

StoichCore::StoichCore()
	: 
		useOneWay_( 0 ),
		path_( "" ),
		objMapStart_( 0 ),
		numVarPools_( 0 ),
		numVarPoolsBytes_( 0 ),
		numBufPools_( 0 ),
		numFuncPools_( 0 ),
		numReac_( 0 )
{;}

StoichCore::~StoichCore()
{
	unZombifyModel();
	for ( vector< RateTerm* >::iterator i = rates_.begin();
		i != rates_.end(); ++i )
		delete *i;

	/*
	 * Do NOT delete FuncTerms, they are just pointers stolen from
	 * the non-zombified objects.
	for ( vector< FuncTerm* >::iterator i = funcs_.begin();
		i != funcs_.end(); ++i )
		delete *i;
		*/
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void StoichCore::setOneWay( bool v )
{
	useOneWay_ = v;
}

bool StoichCore::getOneWay() const
{
	return useOneWay_;
}

void StoichCore::setPath( const Eref& e, const Qinfo* q, string v )
{
	if ( path_ != "" && path_ != v ) {
		// unzombify( path_ );
		cout << "StoichCore::setPath: need to clear old path.\n";
		return;
	}
	path_ = v;
	vector< Id > elist;
	vector< Id > offSolverPools;
	Shell::wildcard( path_, elist );

	Id myCompt = getCompt( e.id() );

	if ( myCompt != Id() ) // Off solver only happens if compt is defined.
		locateOffSolverReacs( myCompt, elist );
	allocateObjMap( elist );
	allocateModel( elist );
	ObjId pa = Neutral::parent( e );
	if ( pa.element()->cinfo()->findFinfo( "stoich" ) )
		SetGet1< Id >::setRepeat( pa.id, "stoich", e.id() );
	zombifyModel( e, elist );
}

string StoichCore::getPath( const Eref& e, const Qinfo* q ) const
{
	return path_;
}

double StoichCore::getEstimatedDt() const
{
	return 1; // Dummy
}

unsigned int StoichCore::getNumVarPools() const
{
	return numVarPools_;
}

unsigned int StoichCore::getNumAllPools() const
{
	assert( diffConst_.size() == 
					numVarPools_ + numBufPools_ + numFuncPools_ );
	return numVarPools_ + numBufPools_ + numFuncPools_;
}

//////////////////////////////////////////////////////////////
// Model zombification functions
//////////////////////////////////////////////////////////////
//
// Simply traverses the tree toward the root till it finds a
// compartment. Pools use a special msg, but this works for reacs too.
Id getCompt( Id id )
{
	const Element* e = id.element();
	if ( e->cinfo()->isA( "PoolBase" ) ) {
		vector< Id > neighbours;
		if ( e->getNeighbours( neighbours, e->cinfo()->findFinfo( "requestSize" ) ) == 1 ) {
			Id pa = Neutral::parent( neighbours[0].eref() ).id;
			if ( pa.element()->cinfo()->isA( "ChemMesh" ) )
				return pa;
		}
	}
	Id pa = Neutral::parent( id.eref() ).id;
	if ( pa == Id() )
		return pa;
	else if ( pa.element()->cinfo()->isA( "ChemMesh" ) )
		return pa;
	return getCompt( pa );
}

static bool isOffSolverReac( const Element* e, Id myCompt, 
				vector< Id >& offSolverPools )
{
	assert( myCompt != Id() );
	assert( myCompt.element()->cinfo()->isA( "ChemMesh" ) );
	bool ret = false;
	vector< Id > neighbours;
	e->getNeighbours( neighbours, e->cinfo()->findFinfo( "toSub" ));
	vector< Id > n2;
	e->getNeighbours( n2, e->cinfo()->findFinfo( "toPrd" ));
	neighbours.insert( neighbours.end(), n2.begin(), n2.end() );
	for ( vector< Id >::const_iterator 
			j = neighbours.begin(); j != neighbours.end(); ++j )
	{
		Id otherCompt = getCompt( *j );
		if ( myCompt != otherCompt ) {
			offSolverPools.push_back( *j );
			ret = true;
		}
	}
	return ret;
}

void StoichCore::locateOffSolverReacs( Id myCompt, vector< Id >& elist )
{
	offSolverPools_.clear();
	offSolverReacs_.clear();

	vector< Id > temp;
	temp.reserve( elist.size() );
	for ( vector< Id >::const_iterator 
					i = elist.begin(); i != elist.end(); ++i )
	{
		const Element* e = i->element();
		if ( e->cinfo()->isA( "ReacBase" ) || e->cinfo()->isA( "EnzBase" ) )
	   	{
			if ( isOffSolverReac( e, myCompt, offSolverPools_ )  )
				offSolverReacs_.push_back( *i );
			else 
				temp.push_back( *i );
		} else {
			temp.push_back( *i );
		}
	}
	elist = temp;
}

void StoichCore::allocateObjMap( const vector< Id >& elist )
{
	vector< Id > temp( elist );
	temp.insert( temp.end(), offSolverPools_.begin(), 
					offSolverPools_.end() );
	temp.insert( temp.end(), offSolverReacs_.begin(), 
					offSolverReacs_.end() );
	objMapStart_ = ~0;
	unsigned int maxId = 0;
	for ( vector< Id >::const_iterator 
					i = temp.begin(); i != temp.end(); ++i ) {
		if ( objMapStart_ > i->value() )
			objMapStart_ = i->value();
		if ( maxId < i->value() )
			maxId = i->value();
	}
	objMap_.clear();
	objMap_.resize( 1 + maxId - objMapStart_, 0 );
	assert( objMap_.size() >= temp.size() );
}

/// Identifies and allocates objects in the StoichCore.
void StoichCore::allocateModelObject( Id id, 
				vector< Id >& bufPools, vector< Id >& funcPools )
{
	static const Cinfo* poolCinfo = Pool::initCinfo();
	static const Cinfo* bufPoolCinfo = BufPool::initCinfo();
	static const Cinfo* funcPoolCinfo = FuncPool::initCinfo();
	static const Cinfo* reacCinfo = Reac::initCinfo();
	static const Cinfo* enzCinfo = Enz::initCinfo();
	static const Cinfo* mmEnzCinfo = MMenz::initCinfo();
	static const Cinfo* sumFuncCinfo = SumFunc::initCinfo();

	Element* ei = id.element();
	if ( ei->cinfo() == poolCinfo ) {
		objMap_[ id.value() - objMapStart_ ] = numVarPools_;
		idMap_.push_back( id );
		++numVarPools_;
	} else if ( ei->cinfo() == bufPoolCinfo ) {
			bufPools.push_back( id );
	} else if ( ei->cinfo() == funcPoolCinfo ) {
			funcPools.push_back( id );
	} else if ( ei->cinfo() == mmEnzCinfo ){
			mmEnzMap_.push_back( ei->id() );
			objMap_[ id.value() - objMapStart_ ] = numReac_;
			++numReac_;
	} else if ( ei->cinfo() == reacCinfo ) {
			reacMap_.push_back( ei->id() );
			if ( useOneWay_ ) {
				objMap_[ id.value() - objMapStart_ ] = numReac_;
				numReac_ += 2;
			} else {
				objMap_[ id.value() - objMapStart_ ] = numReac_;
				++numReac_;
			}
	} else if ( ei->cinfo() == enzCinfo ) {
			enzMap_.push_back( ei->id() );
			if ( useOneWay_ ) {
				objMap_[ id.value() - objMapStart_ ] = numReac_;
				numReac_ += 3;
			} else {
				objMap_[ id.value() - objMapStart_ ] = numReac_;
				numReac_ += 2;
			}
	} else if ( ei->cinfo() == sumFuncCinfo ){
			objMap_[ id.value() - objMapStart_ ] = numFuncPools_;
			++numFuncPools_;
	} 
}

/// Using the computed array sizes, now allocate space for them.
void StoichCore::resizeArrays()
{
	/*
	S_.resize( 1 );
	Sinit_.resize( 1 );
	S_[0].resize( numVarPools_ + numBufPools_ + numFuncPools_, 0.0 );
	Sinit_[0].resize( numVarPools_ + numBufPools_ + numFuncPools_, 0.0);
	*/
	assert( idMap_.size() == numVarPools_ + numBufPools_ + numFuncPools_ +
					offSolverPools_.size() );

	diffConst_.resize( numVarPools_ + numBufPools_ + numFuncPools_, 0.0 );
	species_.resize( numVarPools_ + numBufPools_ + numFuncPools_, 0 );
	rates_.resize( numReac_ );
	// v_.resize( numReac_, 0.0 ); // v is now allocated dynamically
	funcs_.resize( numFuncPools_ );
	N_.setSize( idMap_.size(), numReac_ );
}

/// Calculate sizes of all arrays, and allocate them.
void StoichCore::allocateModel( const vector< Id >& elist )
{
	numVarPools_ = 0;
	numReac_ = 0;
	numFuncPools_ = 0;
	vector< Id > bufPools;
	vector< Id > funcPools;
	idMap_.clear();
	reacMap_.clear();
	enzMap_.clear();
	mmEnzMap_.clear();

	for ( vector< Id >::const_iterator i = elist.begin(); 
					i != elist.end(); ++i )
			allocateModelObject( *i, bufPools, funcPools );
	offSolverRatesOffset_ = numReac_;
	offSolverPoolOffset_ = numVarPools_;
	for ( vector< Id >::const_iterator i = offSolverReacs_.begin(); 
					i != offSolverReacs_.end(); ++i )
			allocateModelObject( *i, bufPools, funcPools );

	numBufPools_ = 0;
	for ( vector< Id >::const_iterator i = bufPools.begin(); i != bufPools.end(); ++i ){
		objMap_[ i->value() - objMapStart_ ] = numVarPools_ + numBufPools_;
		idMap_.push_back( *i );
		++numBufPools_;
	}

	numFuncPools_ = numVarPools_ + numBufPools_;
	for ( vector< Id >::const_iterator i = funcPools.begin(); 
		i != funcPools.end(); ++i ) {
		objMap_[ i->value() - objMapStart_ ] = numFuncPools_++;
		idMap_.push_back( *i );
	}

	unsigned int numOffSolverPools = idMap_.size();
	for ( vector< Id >::const_iterator i = offSolverPools_.begin(); 
					i != offSolverPools_.end(); ++i ) {
		objMap_[ i->value() - objMapStart_ ] = numOffSolverPools++;
		idMap_.push_back( *i );
	}


	assert( idMap_.size() == numOffSolverPools );
	numFuncPools_ -= numVarPools_ + numBufPools_;
	numVarPoolsBytes_ = numVarPools_ * sizeof( double );

	resizeArrays();
}

static void zombifyAndUnschedPool( 
	const Eref& s, Element* orig, const Cinfo* zClass )
{
	////////////////////////////////////////////////////////
	// Unschedule: Get rid of Process message
	static const Finfo* procDest = 
		PoolBase::initCinfo()->findFinfo( "process");
	assert( procDest );

	const DestFinfo* df = dynamic_cast< const DestFinfo* >( procDest );
	assert( df );
	MsgId mid = orig->findCaller( df->getFid() );
	if ( mid != Msg::bad )
		Msg::deleteMsg( mid );

	// Complete the unzombification.
	PoolBase::zombify( orig, zClass, s.id() );
}

void StoichCore::installAndUnschedFunc( Id func, Id Pool )
{
	// Unsched Func
	static const Finfo* procDest = 
		FuncBase::initCinfo()->findFinfo( "process");
	assert( procDest );

	const DestFinfo* df = dynamic_cast< const DestFinfo* >( procDest );
	assert( df );
	MsgId mid = func.element()->findCaller( df->getFid() );
	if ( mid != Msg::bad )
		Msg::deleteMsg( mid );


	// Install the FuncTerm
	static const Finfo* funcSrcFinfo = 
			FuncBase::initCinfo()->findFinfo( "input" );
	FuncBase* fb = reinterpret_cast< FuncBase* >( func.eref().data() );
	FuncTerm* ft = fb->func();
	vector< Id > srcPools;
	unsigned int numSrc = func.element()->getNeighbours( 
					srcPools, funcSrcFinfo );
	assert( numSrc > 0 );
	vector< unsigned int > poolIndex( numSrc, 0 );
	for ( unsigned int i = 0; i < numSrc; ++i )
		poolIndex[i] = convertIdToPoolIndex( srcPools[i] );
	ft->setReactants( poolIndex );
	unsigned int funcIndex = convertIdToFuncIndex( func );
	funcs_[ funcIndex ] = ft;
	// Somewhere I have to tie the output of the FuncTerm to the funcPool.
}

void StoichCore::buildDiffTerms( map< string, unsigned int >& diffTerms ) 
		const
{
	assert( diffConst_.size() == numVarPools_ );
	assert( diffConst_.size() <= idMap_.size() );
	diffTerms.clear();
	for ( unsigned int i = 0; i < diffConst_.size(); ++i )
	{
		if ( diffConst_[i] > 0 ) {
			string name = idMap_[i].element()->getName();
			map< string, unsigned int >::iterator old = 
					diffTerms.find( name );
			if ( old != diffTerms.end() ) {
				cout << "Warning: StoichCore::buildDiffTerms: "
						"multiple pools named '" <<
						name << endl;
				continue;
			}
			diffTerms[name] = i;
		}
	}
}

// e is the stoich Eref, elist is list of all Ids to zombify.
void StoichCore::zombifyModel( const Eref& e, const vector< Id >& elist )
{
	static const Cinfo* poolCinfo = Pool::initCinfo();
	static const Cinfo* bufPoolCinfo = BufPool::initCinfo();
	static const Cinfo* funcPoolCinfo = FuncPool::initCinfo();
	static const Cinfo* reacCinfo = Reac::initCinfo();
	static const Cinfo* enzCinfo = Enz::initCinfo();
	static const Cinfo* mmEnzCinfo = MMenz::initCinfo();
	vector< Id > meshEntries;

	ObjId stoichParent = Neutral::parent( e );
	assert( stoichParent.element()->cinfo()->isA( "GslStoich" ) ||
		stoichParent.element()->cinfo()->isA( "GssaStoich" ) );

	for ( vector< Id >::const_iterator i = elist.begin(); i != elist.end(); ++i ){
		Element* ei = (*i)();
		if ( ei->cinfo() == poolCinfo ) {
			zombifyAndUnschedPool( stoichParent.eref(), 
							(*i)(), ZPool::initCinfo() );
		}
		else if ( ei->cinfo() == bufPoolCinfo ) {
			zombifyAndUnschedPool( stoichParent.eref(), 
							(*i)(), ZBufPool::initCinfo() );
		}
		else if ( ei->cinfo() == funcPoolCinfo ) {
			zombifyAndUnschedPool( stoichParent.eref(), 
							(*i)(), ZFuncPool::initCinfo());
			// Has also got to zombify the Func.
			Id funcId = Neutral::child( i->eref(), "func" );
			assert( funcId != Id() );
			assert( funcId()->cinfo()->isA( "FuncBase" ) );
			installAndUnschedFunc( funcId, (*i) );
		}
		else if ( ei->cinfo() == reacCinfo ) {
			ReacBase::zombify( ei, ZReac::initCinfo(), e.id() );
		}
		else if ( ei->cinfo() == mmEnzCinfo ) {
			EnzBase::zombify( ei, ZMMenz::initCinfo(), e.id() );
		}
		else if ( ei->cinfo() == enzCinfo ) {
			CplxEnzBase::zombify( ei, ZEnz::initCinfo(), e.id() );
		}
	}
}

void StoichCore::unZombifyPools()
{
	unsigned int i = 0;
	for ( ; i < numVarPools_; ++i ) {
		Element* e = idMap_[i].element();
		if ( e != 0 &&  e->cinfo() == ZPool::initCinfo() )
			PoolBase::zombify( e, Pool::initCinfo(), Id() );
	}
	
	for ( ; i < numVarPools_ + numBufPools_; ++i ) {
		Element* e = idMap_[i].element();
		if ( e != 0 &&  e->cinfo() == ZBufPool::initCinfo() )
			PoolBase::zombify( e, BufPool::initCinfo(), Id() );
	}
}

void StoichCore::unZombifyFuncs()
{
	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
	unsigned int start = numVarPools_ + numBufPools_;
	for ( unsigned int k = 0; k < numFuncPools_; ++k ) {
		unsigned int i = k + start;
		Element* e = idMap_[i].element();
		if ( e != 0 &&  e->cinfo() == ZFuncPool::initCinfo() ) {
			PoolBase::zombify( e, FuncPool::initCinfo(), Id() );
			// Has also got to unzombify the Func.
			Id funcId = Neutral::child( idMap_[i].eref(), "func" );
			if ( funcId != Id() ) {
				assert ( funcId()->cinfo()->isA( "FuncBase" ) );
				MsgId mid = s->doAddMsg( "OneToAll", 
				ObjId( 2, 5), "proc5", funcId, "proc" );
				assert( mid != Msg::bad );
				// reschedFunc( funcId.element() );
			}
		}
	}
}

void StoichCore::unZombifyModel()
{
	assert (idMap_.size() == numVarPools_ + numBufPools_ + numFuncPools_ +
					offSolverPools_.size() );

	unZombifyPools();
	unZombifyFuncs();

	Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );

	for ( vector< Id >::iterator i = reacMap_.begin(); 
						i != reacMap_.end(); ++i ) {
		Element* e = i->element();
		if ( e != 0 &&  e->cinfo() == ZReac::initCinfo() )
			ReacBase::zombify( e, Reac::initCinfo(), Id() );
	}
	
	for ( vector< Id >::iterator i = mmEnzMap_.begin(); 
						i != mmEnzMap_.end(); ++i ) {
		Element* e = i->element();
		if ( e != 0 &&  e->cinfo() == ZMMenz::initCinfo() )
			EnzBase::zombify( e, MMenz::initCinfo(), Id() );
	}
	
	for ( vector< Id >::iterator i = enzMap_.begin(); 
						i != enzMap_.end(); ++i ) {
		Element* e = i->element();
		if ( e != 0 &&  e->cinfo() == ZEnz::initCinfo() )
			CplxEnzBase::zombify( e, Enz::initCinfo(), Id() );
	}

	vector< Id > temp( idMap_.begin(), 
			idMap_.begin() + numVarPools_ + numBufPools_ + numFuncPools_ );
	s->addClockMsgs( temp, "proc", 4 );
}

unsigned int StoichCore::convertIdToPoolIndex( Id id ) const
{
	unsigned int i = id.value() - objMapStart_;
	assert( i < objMap_.size() );
	i = objMap_[i];
	assert( i < idMap_.size() );
	return i;
}

unsigned int StoichCore::convertIdToReacIndex( Id id ) const
{
	unsigned int i = id.value() - objMapStart_;
	assert( i < objMap_.size() );
	i = objMap_[i];
	assert( i < rates_.size() );
	return i;
}

unsigned int StoichCore::convertIdToFuncIndex( Id id ) const
{
	unsigned int i = id.value() - objMapStart_;
	assert( i < objMap_.size() );
	i = objMap_[i];
	assert( i < funcs_.size() );
	return i;
}

void StoichCore::installReaction( ZeroOrder* forward, ZeroOrder* reverse, Id reacId )
{
	unsigned int rateIndex = convertIdToReacIndex( reacId );
	unsigned int revRateIndex = rateIndex;
	if ( useOneWay_ ) {
		rates_[ rateIndex ] = forward;
		revRateIndex = rateIndex + 1;
		rates_[ revRateIndex ] = reverse;
	} else {
		rates_[ rateIndex ] = 
			new BidirectionalReaction( forward, reverse );
	}

	vector< unsigned int > molIndex;

	if ( useOneWay_ ) {
		unsigned int numReactants = forward->getReactants( molIndex );
		for ( unsigned int i = 0; i < numReactants; ++i ) {
			int temp = N_.get( molIndex[i], rateIndex );
			N_.set( molIndex[i], rateIndex, temp - 1 );
			temp = N_.get( molIndex[i], revRateIndex );
			N_.set( molIndex[i], revRateIndex, temp + 1 );
		}

		numReactants = reverse->getReactants( molIndex );
		for ( unsigned int i = 0; i < numReactants; ++i ) {
			int temp = N_.get( molIndex[i], rateIndex );
			N_.set( molIndex[i], rateIndex, temp + 1 );
			temp = N_.get( molIndex[i], revRateIndex );
			N_.set( molIndex[i], revRateIndex, temp - 1 );
		}
	} else {
		unsigned int numReactants = forward->getReactants( molIndex );
		for ( unsigned int i = 0; i < numReactants; ++i ) {
			int temp = N_.get( molIndex[i], rateIndex );
			N_.set( molIndex[i], rateIndex, temp - 1 );
		}

		numReactants = reverse->getReactants( molIndex );
		for ( unsigned int i = 0; i < numReactants; ++i ) {
			int temp = N_.get( molIndex[i], revRateIndex );
			N_.set( molIndex[i], rateIndex, temp + 1 );
		}
	}
}

void StoichCore::installMMenz( MMEnzymeBase* meb, unsigned int rateIndex,
	const vector< Id >& subs, const vector< Id >& prds )
{
	rates_[rateIndex] = meb;

	for ( unsigned int i = 0; i < subs.size(); ++i ) {
		unsigned int poolIndex = convertIdToPoolIndex( subs[i] );
		int temp = N_.get( poolIndex, rateIndex );
		N_.set( poolIndex, rateIndex, temp - 1 );
	}
	for ( unsigned int i = 0; i < prds.size(); ++i ) {
		unsigned int poolIndex = convertIdToPoolIndex( prds[i] );
		int temp = N_.get( poolIndex, rateIndex );
		N_.set( poolIndex, rateIndex, temp + 1 );
	}
}

void StoichCore::installEnzyme( ZeroOrder* r1, ZeroOrder* r2, ZeroOrder* r3,
	Id enzId, Id enzMolId, const vector< Id >& prds ) 
{
	unsigned int rateIndex = convertIdToReacIndex( enzId );

	if ( useOneWay_ ) {
		rates_[ rateIndex ] = r1;
		rates_[ rateIndex + 1 ] = r2;
		rates_[ rateIndex + 2 ] = r3;
	} else {
		rates_[ rateIndex ] = new BidirectionalReaction( r1, r2 );
		rates_[ rateIndex + 1 ] = r3;
	}

	vector< unsigned int > poolIndex;
	unsigned int numReactants = r2->getReactants( poolIndex );
	assert( numReactants == 1 ); // Should be cplx as the only product
	unsigned int cplxPool = poolIndex[0];

	if ( useOneWay_ ) {
		numReactants = r1->getReactants( poolIndex ); // Substrates
		for ( unsigned int i = 0; i < numReactants; ++i ) {
			int temp = N_.get( poolIndex[i], rateIndex ); // terms for r1
			N_.set( poolIndex[i], rateIndex, temp - 1 );
			temp = N_.get( poolIndex[i], rateIndex + 1 ); //terms for r2
			N_.set( poolIndex[i], rateIndex + 1, temp + 1 );
		}

		int temp = N_.get( cplxPool, rateIndex );	// term for r1
		N_.set( cplxPool, rateIndex, temp + 1 );
		temp = N_.get( cplxPool, rateIndex + 1 );	// term for r2
		N_.set( cplxPool, rateIndex + 1, temp -1 );
	} else { // Regular bidirectional reactions.
		numReactants = r1->getReactants( poolIndex ); // Substrates
		for ( unsigned int i = 0; i < numReactants; ++i ) {
			int temp = N_.get( poolIndex[i], rateIndex );
			N_.set( poolIndex[i], rateIndex, temp - 1 );
		}
		int temp = N_.get( cplxPool, rateIndex );
		N_.set( cplxPool, rateIndex, temp + 1 );
	}

	// Now assign reaction 3. The complex is the only substrate here.
	// Reac 3 is already unidirectional, so all we need to do to handle
	// one-way reactions is to get the index right.
	unsigned int reac3index = ( useOneWay_ ) ? rateIndex + 2 : rateIndex + 1;
	int temp = N_.get( cplxPool, reac3index );
	N_.set( cplxPool, reac3index, temp - 1 );

	// For the products, we go to the prd list directly.
	for ( unsigned int i = 0; i < prds.size(); ++i ) {
		unsigned int j = convertIdToPoolIndex( prds[i] );
		int temp = N_.get( j, reac3index );
		N_.set( j, reac3index, temp + 1 );
	}
	// Enz is also a product here.
	unsigned int enzPool = convertIdToPoolIndex( enzMolId );
	temp = N_.get( enzPool, reac3index );
	N_.set( enzPool, reac3index, temp + 1 );
}

//////////////////////////////////////////////////////////////
// Field interface functions
//////////////////////////////////////////////////////////////

/**
 * Sets the forward rate v (given in millimoloar concentration units)
 * for the specified reaction throughout the compartment in which the
 * reaction lives. Internally the stoich uses #/voxel units so this 
 * involves querying the volume subsystem about volumes for each
 * voxel, and scaling accordingly.
 * For now assume a uniform voxel volume and hence just convert on 
 * 0 meshIndex.
 */
void StoichCore::setReacKf( const Eref& e, double v ) const
{
	static const SrcFinfo* toSub = dynamic_cast< const SrcFinfo* > (
		ZReac::initCinfo()->findFinfo( "toSub" ) );

	assert( toSub );
	double volScale = convertConcToNumRateUsingMesh( e, toSub, 0 );

	rates_[ convertIdToReacIndex( e.id() ) ]->setR1( v / volScale );
}

/**
 * For now assume a single rate term.
 */
void StoichCore::setReacKb( const Eref& e, double v ) const
{
	static const SrcFinfo* toPrd = static_cast< const SrcFinfo* > (
		ZReac::initCinfo()->findFinfo( "toPrd" ) );

	assert( toPrd );
	double volScale = convertConcToNumRateUsingMesh( e, toPrd, 0 );


	if ( useOneWay_ )
		 rates_[ convertIdToReacIndex( e.id() ) + 1 ]->setR1( v / volScale);
	else
		 rates_[ convertIdToReacIndex( e.id() ) ]->setR2( v / volScale );
}

void StoichCore::setMMenzKm( const Eref& e, double v ) const
{
	static const SrcFinfo* toSub = dynamic_cast< const SrcFinfo* > (
		ZMMenz::initCinfo()->findFinfo( "toSub" ) );
	// Identify MMenz rate term
	RateTerm* rt = rates_[ convertIdToReacIndex( e.id() ) ];
	MMEnzymeBase* enz = dynamic_cast< MMEnzymeBase* >( rt );
	assert( enz );
	// Identify MMenz Enzyme substrate. I would have preferred the parent,
	// but that gets messy.
	// unsigned int enzMolIndex = enz->getEnzIndex();

	// This function can be replicated to handle multiple different voxels.
	vector< double > vols;
	getReactantVols( e, toSub, vols );
	if ( vols.size() == 0 ) {
		cerr << "Error: StoichCore::setMMenzKm: no substrates for enzyme " <<
			e << endl;
		return;
	}
	// Do scaling and assignment.
	enz->setR1( v * vols[0] * NA );
}

void StoichCore::setMMenzKcat( const Eref& e, double v ) const
{
	RateTerm* rt = rates_[ convertIdToReacIndex( e.id() ) ];
	MMEnzymeBase* enz = dynamic_cast< MMEnzymeBase* >( rt );
	assert( enz );

	enz->setR2( v );
}

/// Later handle all the volumes when this conversion is done.
void StoichCore::setEnzK1( const Eref& e, double v ) const
{
	static const SrcFinfo* toSub = dynamic_cast< const SrcFinfo* > (
		ZEnz::initCinfo()->findFinfo( "toSub" ) );
	assert( toSub );

	double volScale = convertConcToNumRateUsingMesh( e, toSub, 1 );

	rates_[ convertIdToReacIndex( e.id() ) ]->setR1( v / volScale );
}

void StoichCore::setEnzK2( const Eref& e, double v ) const
{
	if ( useOneWay_ )
		rates_[ convertIdToReacIndex( e.id() ) + 1 ]->setR1( v );
	else
		rates_[ convertIdToReacIndex( e.id() ) ]->setR2( v );
}

void StoichCore::setEnzK3( const Eref& e, double v ) const
{
	if ( useOneWay_ )
		rates_[ convertIdToReacIndex( e.id() ) + 2 ]->setR1( v );
	else
		rates_[ convertIdToReacIndex( e.id() ) + 1 ]->setR1( v );
}

/**
 * Looks up the matching rate for R1. Later we may have additional 
 * scaling terms for the specified voxel.
 */
double StoichCore::getR1( unsigned int reacIndex, unsigned int voxel ) const
{
	return rates_[ reacIndex ]->getR1();
}

/**
 * Looks up the matching rate for R2. Later we may have additional 
 * scaling terms for the specified voxel.
 */
double StoichCore::getR2( unsigned int reacIndex, unsigned int voxel ) const
{
	return rates_[ reacIndex ]->getR2();
}

double StoichCore::getDiffConst( unsigned int p ) const
{
	assert( p < diffConst_.size() );
	return diffConst_[p];
}

void StoichCore::setDiffConst( unsigned int p, double d )
{
	assert( p < diffConst_.size() );
	if ( d < 0 ) {
		cout << "Warning: StoichCore::setDiffConst: D[" << p << 
			"] cannot be -ve: " << d << endl;
		return;
	}
	diffConst_[p] = d;
}

SpeciesId StoichCore::getSpecies( unsigned int poolIndex ) const
{
	return species_[ poolIndex ];
}

void StoichCore::setSpecies( unsigned int poolIndex, SpeciesId s )
{
	species_[ poolIndex ] = s;
}

// for debugging.
void StoichCore::print() const
{
	N_.print();
}

/////////////////////////////////////////////////////////////////////
const vector< Id >& StoichCore::getOffSolverPools() const
{
	return offSolverPools_;
}

/////////////////////////////////////////////////////////////////////
// Numeric funcs. These are in StoichCore because the rate terms are here.
/////////////////////////////////////////////////////////////////////

/**
 * updateRates computes the velocity *v* of each reaction. Then it
 * uses this to compute the rate of change, *yprime*, for each pool
 */

void StoichCore::updateRates( const double* s, double* yprime )
{
	vector< RateTerm* >::const_iterator i;
	vector< double > v( numReac_, 0.0 );
	vector< double >::iterator j = v.begin();
	assert( numReac_ == rates_.size() );

	for ( i = rates_.begin(); i != rates_.end(); i++) {
		*j++ = (**i)( s );
		assert( !isnan( *( j-1 ) ) );
	}

	for (unsigned int i = 0; i < numVarPools_; ++i)
		*yprime++ = N_.computeRowRate( i , v );
}

// s is the array of pools, S_[meshIndex][0]
void StoichCore::updateFuncs( double* s, double t )
{
	double* j = s + numVarPools_ + numBufPools_;

	for ( vector< FuncTerm* >::iterator i = funcs_.begin();
					i != funcs_.end(); ++i ) {
		*j++ = (**i)( s, t );
		assert( !isnan( *(j-1) ) );
	}
}

/**
 * updateJunctionRates:
 * Updates the rates for cross-compartment reactions. These are located
 * at the end of the rates_ vector, and are directly indext by the the
 * reacTerms.
 */
void StoichCore::updateJunctionRates( const double* s,
	const vector< unsigned int >& reacTerms, double* yprime )
{
	for ( vector< unsigned int >::const_iterator i = reacTerms.begin(); 
					i != reacTerms.end(); ++i )
	{
			assert( *i < rates_.size() );
			*yprime++ += (*rates_[*i])( s );
	}
}

