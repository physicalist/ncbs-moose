/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include <set>
#include "SpikeGen.h"       // for generating spikes
#include <queue>            // for SynChanStruct in BioScan. Remove eventually.
#include "HSolveStruct.h"
#include "BioScan.h"
#include "HinesMatrix.h"
#include "HSolvePassive.h"
#include "RateLookup.h"
#include "HSolveActive.h"

const vector< Id >& HSolveActive::getCompartments( ) const
{
	return compartmentId_;
}

double HSolveActive::getVm( unsigned int index ) const
{
	assert( index < V_.size() );
	
	if ( index < V_.size() )
		return V_[ index ];
	
	return 0.0;
}

void HSolveActive::setVm( unsigned int index, double value )
{
	assert( index < V_.size() );
	
	if ( index < V_.size() )
		V_[ index ] = value;
}

double HSolveActive::getInject( unsigned int index ) const
{
	assert( index < nCompt_ );
	
	if ( index < nCompt_ )
		return compartment_[ index ].inject;
	
	return 0.0;
}

void HSolveActive::setInject( unsigned int index, double value )
{
	assert( index < nCompt_ );
	
	if ( index < nCompt_ )
		compartment_[ index ].inject = value;
}

double HSolveActive::getIm( unsigned int index ) const
{
	assert( index < nCompt_ );
	
	if ( index >= nCompt_ )
		return 0.0;
	
	double Im =
		compartment_[ index ].EmByRm - V_[ index ] / tree_[ index ].Rm;
	
	vector< CurrentStruct >::const_iterator icurrent;
	
	if ( index == 0 )
		icurrent = current_.begin();
	else
		icurrent = currentBoundary_[ index - 1 ];
	
	for ( ; icurrent < currentBoundary_[ index ]; ++icurrent )
		Im += ( icurrent->Ek - V_[ index ] ) * icurrent->Gk;
	
	return Im;
}
