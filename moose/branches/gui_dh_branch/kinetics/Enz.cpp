/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "ElementValueFinfo.h"
#include "lookupSizeFromMesh.h"
#include "Enz.h"

#define EPSILON 1e-15

static SrcFinfo2< double, double > *toSub() {
	static SrcFinfo2< double, double > toSub( 
			"toSub", 
			"Sends out increment of molecules on product each timestep"
			);
	return &toSub;
}

static SrcFinfo2< double, double > *toPrd() {
	static SrcFinfo2< double, double > toPrd( 
			"toPrd", 
			"Sends out increment of molecules on product each timestep"
			);
	return &toPrd;
}

static SrcFinfo2< double, double > *toEnz() {
	static SrcFinfo2< double, double > toEnz( 
			"toEnz", 
			"Sends out increment of molecules on product each timestep"
			);
	return &toEnz;
}

static SrcFinfo2< double, double > *toCplx() {
	static SrcFinfo2< double, double > toCplx( 
			"toCplx", 
			"Sends out increment of molecules on product each timestep"
			);
	return &toCplx;
}

const Cinfo* Enz::initCinfo()
{
		//////////////////////////////////////////////////////////////
		// Field Definitions
		//////////////////////////////////////////////////////////////
		static ElementValueFinfo< Enz, double > k1(
			"k1",
			"Forward reaction from enz + sub to complex",
			&Enz::setK1,
			&Enz::getK1
		);

		static ValueFinfo< Enz, double > k2(
			"k2",
			"Reverse reaction from complex to enz + sub",
			&Enz::setK2,
			&Enz::getK2
		);

		static ValueFinfo< Enz, double > k3(
			"k3",
			"Forward rate constant from complex to product + enz",
			&Enz::setK3,
			&Enz::getK3
		);

		static ElementValueFinfo< Enz, double > Km(
			"Km",
			"Michaelis-Menten constant in concentration (millimolar) units",
			&Enz::setKm,
			&Enz::getKm
		);

		static ValueFinfo< Enz, double > kcat(
			"kcat",
			"Forward reaction rate for enzyme, equivalent to k3.",
			&Enz::setK3,
			&Enz::getK3
		);

		static ElementValueFinfo< Enz, double > ratio(
			"ratio",
			"Ratio of k2/k3",
			&Enz::setRatio,
			&Enz::getRatio
		);

		static ElementValueFinfo< Enz, double > concK1(
			"concK1",
			"K1 expressed in concentration (1/millimolar.sec) units",
			&Enz::setConcK1,
			&Enz::getConcK1
		);

		static ReadOnlyElementValueFinfo< Enz, unsigned int > numSub(
			"numSubstrates",
			"Number of substrates in this enzyme reaction. Usually 1."
			"Does not include the enzyme itself",
			&Enz::getNumSub
		);

		//////////////////////////////////////////////////////////////
		// MsgDest Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo process( "process",
			"Handles process call",
			new ProcOpFunc< Enz >( &Enz::process ) );
		static DestFinfo reinit( "reinit",
			"Handles reinit call",
			new ProcOpFunc< Enz >( &Enz::reinit ) );

		static DestFinfo group( "group",
			"Handle for group msgs. Doesn't do anything",
			new OpFuncDummy() );

		static DestFinfo remesh( "remesh",
			"Compartment volume or meshing has changed, recompute rates",
			new EpFunc0< Enz >( &Enz::remesh ) );

		//////////////////////////////////////////////////////////////
		// Shared Msg Definitions
		//////////////////////////////////////////////////////////////
		static DestFinfo subDest( "subDest",
				"Handles # of molecules of substrate",
				new OpFunc1< Enz, double >( &Enz::sub ) );
		static DestFinfo enzDest( "enzDest",
				"Handles # of molecules of Enzyme",
				new OpFunc1< Enz, double >( &Enz::enz ) );
		static DestFinfo prdDest( "prdDest",
				"Handles # of molecules of product. Dummy.",
				new OpFunc1< Enz, double >( &Enz::prd ) );
		static DestFinfo cplxDest( "cplxDest",
				"Handles # of molecules of enz-sub complex",
				new OpFunc1< Enz, double >( &Enz::cplx ) );
		static Finfo* subShared[] = {
			toSub(), &subDest
		};
		static Finfo* enzShared[] = {
			toEnz(), &enzDest
		};
		static Finfo* prdShared[] = {
			toPrd(), &prdDest
		};
		static Finfo* cplxShared[] = {
			toCplx(), &cplxDest
		};

		static SharedFinfo sub( "sub",
			"Connects to substrate pool",
			subShared, sizeof( subShared ) / sizeof( const Finfo* )
		);
		static SharedFinfo prd( "prd",
			"Connects to product pool",
			prdShared, sizeof( prdShared ) / sizeof( const Finfo* )
		);
		static SharedFinfo enz( "enz",
			"Connects to enzyme pool",
			enzShared, sizeof( enzShared ) / sizeof( const Finfo* )
		);
		static SharedFinfo cplx( "cplx",
			"Connects to enz-sub complex pool",
			cplxShared, sizeof( cplxShared ) / sizeof( const Finfo* )
		);
		static Finfo* procShared[] = {
			&process, &reinit
		};
		static SharedFinfo proc( "proc",
			"Shared message for process and reinit",
			procShared, sizeof( procShared ) / sizeof( const Finfo* )
		);

	static Finfo* enzFinfos[] = {
		&k1,	// Value
		&k2,	// Value
		&k3,	// Value
		&Km,	// Value
		&kcat,	// Value
		&ratio,	// Value
		&concK1,	// Value
		&numSub,	// ReadOnlyValue
		&sub,				// SharedFinfo
		&prd,				// SharedFinfo
		&enz,				// SharedFinfo
		&cplx,				// SharedFinfo
		&proc,				// SharedFinfo
		&remesh,			// DestFinfo
	};

	static Cinfo enzCinfo (
		"Enz",
		Neutral::initCinfo(),
		enzFinfos,
		sizeof( enzFinfos ) / sizeof ( Finfo* ),
		new Dinfo< Enz >()
	);

	return &enzCinfo;
}

 static const Cinfo* enzCinfo = Enz::initCinfo();

//////////////////////////////////////////////////////////////
// Enz internal functions
//////////////////////////////////////////////////////////////


Enz::Enz( )
	: k1_( 0.1 ), k2_( 0.4 ), k3_( 0.1 )
{
	;
}

//////////////////////////////////////////////////////////////
// MsgDest Definitions
//////////////////////////////////////////////////////////////

void Enz::sub( double n )
{
	r1_ *= n;
}

void Enz::prd( double n ) // dummy
{
	;
}

void Enz::enz( double n )
{
	r1_ *= n;
}

void Enz::cplx( double n )
{
	r2_ = k2_ * n;
	r3_ = k3_ * n;
}

void Enz::process( const Eref& e, ProcPtr p )
{
	toSub()->send( e, p->threadIndexInGroup, r2_, r1_ );
	toPrd()->send( e, p->threadIndexInGroup, r3_, 0 );
	toEnz()->send( e, p->threadIndexInGroup, r3_ + r2_, r1_ );
	toCplx()->send( e, p->threadIndexInGroup, r1_, r3_ + r2_ );

	// cout << "	proc: " << r1_ << ", " << r2_ << ", " << r3_ << endl;
	
	r1_ = k1_;
}

void Enz::reinit( const Eref& e, ProcPtr p )
{
	r1_ = k1_;
}

void Enz::remesh( const Eref& e, const Qinfo* q )
{
	setKm( e, q, Km_ );
}

//////////////////////////////////////////////////////////////
// Field Definitions
//////////////////////////////////////////////////////////////

void Enz::setK1( const Eref& e, const Qinfo* q, double v )
{
	r1_ = k1_ = v;
	double volScale = 
		convertConcToNumRateUsingMesh( e, toSub(), 1 );
	Km_ = ( k2_ + k3_ ) / ( k1_ * volScale );
}

double Enz::getK1( const Eref& e, const Qinfo* q ) const
{
	return k1_;
}

void Enz::setK2( double v )
{
	k2_ = v;
}

double Enz::getK2() const
{
	return k2_;
}

void Enz::setK3( double v )
{
	k3_ = v;
}

double Enz::getK3() const
{
	return k3_;
}

//////////////////////////////////////////////////////////////
// Scaled field terms.
// We assume that when we set these, the k1, k2 and k3 vary as needed
// to preserve the other field terms. So when we set Km, then kcat
// and ratio remain unchanged.
//////////////////////////////////////////////////////////////

void Enz::setKm( const Eref& e, const Qinfo* q, double v )
{
	Km_ = v;
	double volScale = 
		convertConcToNumRateUsingMesh( e, toSub(), 1 );
	k1_ = ( k2_ + k3_ ) / ( v * volScale );
}

double Enz::getKm( const Eref& e, const Qinfo* q ) const
{
	return Km_;
}

void Enz::setRatio( const Eref& e, const Qinfo* q, double v )
{
	k2_ = v * k3_;
	double volScale = 
		convertConcToNumRateUsingMesh( e, toSub(), 1 );

	k1_ = ( k2_ + k3_ ) / ( Km_ * volScale );
}

double Enz::getRatio( const Eref& e, const Qinfo* q ) const
{
	return k2_ / k3_;
}

void Enz::setConcK1( const Eref& e, const Qinfo* q, double v )
{
	double volScale = convertConcToNumRateUsingMesh( e, toSub(), 1 );
	r1_ = k1_ = v * volScale;
	Km_ = ( k2_ + k3_ ) / ( k1_ * volScale );
}

double Enz::getConcK1( const Eref& e, const Qinfo* q ) const
{
	double volScale = convertConcToNumRateUsingMesh( e, toSub(), 1 );
	return k1_ * volScale;
}


unsigned int Enz::getNumSub( const Eref& e, const Qinfo* q ) const
{
	const vector< MsgFuncBinding >* mfb = 
		e.element()->getMsgAndFunc( toSub()->getBindIndex() );
	assert( mfb );
	return ( mfb->size() );
}
