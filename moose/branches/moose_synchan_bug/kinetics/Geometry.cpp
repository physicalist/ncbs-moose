/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/


#include "moose.h"
#include "Geometry.h"

/**
 * The Geometry corresponds to the Smoldyn surfacesuperstruct.
 * It manages multiple surfaces that matter to a given solver.
 * It mostly handles a list of surfaces, but has a couple of global control
 * parameters for tolerances, and messages to send these on to the
 * solver when updated.
 */
const Cinfo* initGeometryCinfo()
{
	static Finfo* geomShared[] =
	{
		new SrcFinfo( "epsilonSrc", Ftype1< double >::global() ),
		new SrcFinfo( "neighDistSrc", Ftype1< double >::global() )
	};

	static Finfo* geometryFinfos[] =
	{
	///////////////////////////////////////////////////////
	// Field definitions
	///////////////////////////////////////////////////////
		// epsilon is the max deviation of surface-point from surface
		// I think it refers to when the molecule is stuck to the surface.
		// Need to check with Steven.
		new ValueFinfo( "epsilon", 
			ValueFtype1< double >::global(),
			GFCAST( &Geometry::getEpsilon ), 
			RFCAST( &Geometry::setEpsilon )
		),

		// neighdist is capture distance from one panel to another.
		// When a molecule diffuses off one panel and is within neighdist
		// of the other, it is captured by the second.
		new ValueFinfo( "neighdist", 
			ValueFtype1< double >::global(),
			GFCAST( &Geometry::getNeighDist ), 
			RFCAST( &Geometry::setNeighDist )
		),
	///////////////////////////////////////////////////////
	// MsgSrc definitions
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	// MsgDest definitions
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	// Synapse definitions
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	// Shared definitions
	///////////////////////////////////////////////////////
		new SharedFinfo( "geom", geomShared,
			sizeof( geomShared ) / sizeof( Finfo* ) ),
	};

	static Cinfo geometryCinfo(
		"Geometry",
		"Upinder S. Bhalla, 2007, NCBS",
		"Geometry: Interface object between Smoldyn (by Steven Andrews) and MOOSE, corresponding to the Smoldyn surfacesuperstruct",
		initNeutralCinfo(),
		geometryFinfos,
		sizeof( geometryFinfos )/sizeof(Finfo *),
		ValueFtype1< Geometry >::global()
	);

	return &geometryCinfo;
}

static const Cinfo* geometryCinfo = initGeometryCinfo();

static const unsigned int epsilonSlot =
	initGeometryCinfo()->getSlotIndex( "geom.epsilonSrc" );
static const unsigned int neighDistSlot =
	initGeometryCinfo()->getSlotIndex( "geom.neighDistSrc" );

///////////////////////////////////////////////////
// Class function definitions
///////////////////////////////////////////////////

Geometry::Geometry()
{
		;
}

///////////////////////////////////////////////////
// Field access functions.
///////////////////////////////////////////////////
		
void Geometry::setEpsilon( const Conn& c, double value )
{
	if ( value >= 0.0 )
		static_cast< Geometry* >( c.data() )->epsilon_ = value;
	
	// Send the update over to the solver
	send1< double >( c.targetElement(), epsilonSlot, value );
}

double Geometry::getEpsilon( const Element* e )
{
	return static_cast< Geometry* >( e->data() )->epsilon_;
}

void Geometry::setNeighDist( const Conn& c, double value )
{
	if ( value >= 0.0 )
		static_cast< Geometry* >( c.data() )->neighDist_ = value;
	
	// Send the update over to the solver
	send1< double >( c.targetElement(), neighDistSlot, value );
}

double Geometry::getNeighDist( const Element* e )
{
	return static_cast< Geometry* >( e->data() )->neighDist_;
}

