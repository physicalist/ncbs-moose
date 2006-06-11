/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
********************************************************************* */

#include <vector>
#include <iostream>
using namespace std;

#include "Interpol.h"

double Interpol::interpolateWithoutCheck( double x ) const
{
	double xv = ( x - xmin_ ) * invDx_;
	unsigned long i = static_cast< unsigned long >( xv );
	return table_[ i ] + ( table_[ i + 1 ] - table_ [ i ] ) *
		( xv - i );
}

double Interpol::doLookup( double x ) const
{
	if ( x <= xmin_ ) 
		return table_.front();
	if ( x >= xmax_ )
		return table_.back();
	if ( mode_ == 0 )
		return interpolateWithoutCheck( x );
	else 
		return indexWithoutCheck( x );
}

bool Interpol::operator==( const Interpol& other ) const
{
	return (
		xmin_ == other.xmin_ &&
		xmax_ == other.xmax_ &&
		mode_ == other.mode_ &&
		table_ == other.table_ );
}

bool Interpol::operator<( const Interpol& other ) const
{
	if ( *this == other )
		return 0;
	if ( table_.size() < other.table_.size() )
		return 1;
	if ( table_.size() > other.table_.size() )
		return 0;
	for (size_t i = 0; i < table_.size(); i++) {
		if ( table_[i] < other.table_[i] )
			return 1;
		if ( table_[i] > other.table_[i] )
			return 0;
	}
	return 0;
}

void Interpol::localSetXmin( double value ) {
	if ( fabs( xmax_ - value) > EPSILON ) {
		xmin_ = value;
		invDx_ = static_cast< double >( table_.size() - 1 ) / 
			( xmax_ - xmin_ );
	} else {
		cerr << "Warning: InterpolWrapper: Xmin ~= Xmax : Assignment failed\n";
	}
}
void Interpol::localSetXmax( double value ) {
	if ( fabs( value - xmin_ ) > EPSILON ) {
		xmax_ = value;
		invDx_ = static_cast< double >( table_.size() - 1 ) / 
			( xmax_ - xmin_ );
	} else {
		cerr << "Warning: InterpolWrapper: Xmin ~= Xmax : Assignment failed\n";
	}
}
void Interpol::localSetXdivs( int value ) {
	if ( value > 0 ) {
		table_.resize( value + 1 );
		invDx_ = static_cast< double >( value ) / ( xmax_ - xmin_ );
	}
}
// Later do interpolation etc to preseve contents.
// Later also check that it is OK for xmax_ < xmin_
void Interpol::localSetDx( double value ) {
	if ( fabs( value - EPSILON ) > 0 ) {
		int xdivs = static_cast< int >( 
			0.5 + fabs( xmax_ - xmin_ ) / value );
		if ( xdivs < 1 || xdivs > MAX_DIVS ) {
			cerr << "Warning: InterpolWrapper: Out of range:" <<
				xdivs << " entries in table.\n";
				return;
		}
		table_.resize( xdivs + 1 );
		invDx_ = static_cast< double >( xdivs ) / 
			( xmax_ - xmin_ );
	}
}
double Interpol::localGetDx() const {
	return ( xmax_ - xmin_ ) / static_cast< double >( table_.size() - 1 );
}
