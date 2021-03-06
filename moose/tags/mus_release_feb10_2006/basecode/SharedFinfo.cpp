/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"

void SharedFinfo::initialize( const Cinfo* c )
{
	vector< Finfo* >::iterator i;
	parseTriggerList( c, connSharers_, finfos_ );
	for (i = finfos_.begin(); i != finfos_.end(); i++ ) {
		const string& name = ( *i )->name();
		if ( name.substr( name.length() - 2 ) == "In" )
			sharedIn_.push_back( *i );
		else if ( name.substr( name.length() - 3 ) == "Out" )
			sharedOut_.push_back( *i );
	}
}

unsigned long maxIndexOfMatchingFunc( Element* e,
	vector< Finfo* >& sharedOut, vector< Finfo* >& sharedIn ) 
{
	unsigned long max = 0;
	unsigned long j = 0;
	unsigned long nOut = sharedOut.size();
	for ( unsigned long i = 0; i < nOut; i++ ) {
		j = sharedOut[ i ]->indexOfMatchingFunc(
			e, sharedIn[ i ]->recvFunc() );
		if ( max < j )
			max = j;
	}
	return max;
}

bool SharedFinfo::add( Element* e, Field& destfield, bool useSharedConn)
{
	unsigned long nOut = sharedOut_.size();
	unsigned long nIn = sharedIn_.size();
	Finfo* dest = destfield.respondToAdd( this );
	if ( !dest )
		return 0; // Target does not like me.

	SharedFinfo* destSF = dynamic_cast< SharedFinfo* >( dest );

	if ( destSF ) {
		unsigned long maxout = maxIndexOfMatchingFunc(
			e, sharedOut_, destSF->sharedIn_ );
		unsigned long maxin = maxIndexOfMatchingFunc(
			destfield.getElement(), destSF->sharedOut_, sharedIn_ );
		if ( getConn_( e )->connect(
			dest->inConn( destfield.getElement() ), maxout, maxin )
		) {
			unsigned int i;
			for ( i = 0; i < nOut; i++ ) {
				sharedOut_[ i ]->addRecvFunc( e, destSF->sharedIn_[ i ]->recvFunc(), maxout );
			}
			for (i = 0; i < nIn; i++ ) {
				destSF->sharedOut_[ i ]->addRecvFunc( destfield.getElement(), sharedIn_[ i ]->recvFunc(), maxin );
			}
			return 1;
		}
	} else { // It must be a ValueRelayFinfo. Here we expect the
		// shared finfo to have a single trigger as a msgsrc, and a 
		// single msg of type T as the return value.
		// Somewhat unsatisfactory because the outConn is vacant.
		RecvFunc rf = dest->recvFunc();
		unsigned long max = 0;
		max = sharedOut_[ 0 ]->indexOfMatchingFunc( e, rf );
		if ( getConn_( e )->connect(
			dest->inConn( destfield.getElement() ), max, 0 )
		) {
			sharedOut_[ 0 ]->addRecvFunc( e, rf, max );
			dest->addRecvFunc( destfield.getElement(), sharedIn_[ 0 ]->recvFunc(), 0 );

/*
			Field temp1( dest, destfield.getElement() );
			sharedOut_[ 0 ]->add( e, temp1, 1 );
			Field temp2( sharedIn_[ 0 ], e );
			destfield.getFinfo()->add( destfield.getElement(), 
				temp2, 1);
				*/
			return 1;
		}
	}

	finfoErrorMsg( "SharedFinfo", destfield );
	return 0;
}

Finfo* SharedFinfo::respondToAdd( Element* e, const Finfo* sender )
{
	if ( dynamic_cast< const SharedFinfo* >( sender ) != 0 &&
		isSameType( sender ) )
		return this;
	return 0;
}

///////////////////////////////////////////////////////////////////////
// ReturnFinfo.
// A class for handling shared messages where the incoming message
// triggers a return call to the originating object, but not to
// any other object.
///////////////////////////////////////////////////////////////////////

/*
void ReturnFinfo::src( vector< Field >& list, Element* e )
{
	cout << "Not yet implemented. Need to scan Conn list\n";
}

Finfo* ReturnFinfo::respondToAdd( Element* e, const Finfo* sender )
{

}
*/
