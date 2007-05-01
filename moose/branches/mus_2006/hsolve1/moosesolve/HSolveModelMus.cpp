/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**         copyright (C) 2007 Niraj Dudani, Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <vector>
#include "header.h"
#include "../../zbiophysics/ZGate.h"
#include "../../zbiophysics/ZGateWrapper.h"
#include "../HSolveModel.h"
#include "HSolveModelMus.h"

vector< Element* > HSolveModelMus::neighbours( Element* const& compartment ) const {
	vector< Element* > neighbour;
	vector< Field > c;
	vector< Field >::iterator ic;
	compartment->field( "axialIn" ).src( c );
	compartment->field( "axialOut" ).dest( c );
	for ( ic = c.begin(); ic != c.end(); ++ic )
		neighbour.push_back( ic->getElement() );
	return neighbour;
}

vector< Element* > HSolveModelMus::channels( Element* const& compartment ) const {
	vector< Element* > channel;
	vector< Field > c;
	vector< Field >::iterator ic;
	compartment->field( "channelIn" ).src( c );
	for ( ic = c.begin(); ic != c.end(); ++ic )
		channel.push_back( ic->getElement() );
	return channel;
}

vector< Element* > HSolveModelMus::gates( Element* const& channel ) const {
	vector< Element* > gate;
	vector< Field > c;
	vector< Field >::iterator ic;
	channel->field( "xGateOut" ).dest( c );
	channel->field( "yGateOut" ).dest( c );
	channel->field( "zGateOut" ).dest( c );
	for ( ic = c.begin(); ic != c.end(); ++ic )
		gate.push_back( ic->getElement() );
	return gate;
}

void HSolveModelMus::getFieldDouble( Element* const& object,
	const string& field,
	double& value ) const {
	Ftype1< double >::get( object, field, value );
}

void HSolveModelMus::getFieldInt( Element* const& object,
	const string& field, int& value ) const {
	Ftype1< int >::get( object, field, value );
}

double HSolveModelMus::getAlpha( Element* const& gate, double V ) const {
	return ( dynamic_cast< ZGateWrapper* >( gate )->alpha )( V );
}

double HSolveModelMus::getBeta( Element* const& gate, double V ) const {
	return ( dynamic_cast< ZGateWrapper* >( gate )->beta )( V );
}
