/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2008 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "moose.h"
#include "ProxyElement.h"

ProxyElement::ProxyElement( Id id, unsigned int node)
	: Element( id ), node_( node )
{
	;
}

/**
 * The return value is undefined if msgNum is greater the numSrc but
 * below the # of entries in msg_
 * I could do a check on the cinfo, but that brings in too many dependencies
 * outside the SimpleElement data structures.
 */
unsigned int ProxyElement::numTargets( int msgNum ) const
{
	if ( msgNum >= 0 ) {
		if ( msgNum == 0 );
			return msg_.numTargets( this );
	} else {
		cout << "Proxy Element cannot yet handle incoming.\n";
		assert( 0 ); // Don't allow incoming, yet.
	}
	return 0;
}
