/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"

/**
 * Used to do the serious argument juggling in GetOpFunc::op
 */
void fieldOp( const Eref& e, const char* buf, 
	const char* data, unsigned int size )
{
	const Qinfo* q = reinterpret_cast< const Qinfo* >( buf );
	buf += sizeof( Qinfo );
	FuncId retFunc = *reinterpret_cast< const FuncId* >( buf );

	PrepackedBuffer pb( data, size );

	// Flag arguments: useSendTo = 1, and flip the isForward flag.
	Conv< unsigned int > conv1( Shell::myNode() );
	Conv< unsigned int > conv2( Shell::OkStatus );
	// This costs 2 copy operations. Wasteful, but who cares.
	Conv< PrepackedBuffer > conv3( pb );
	unsigned int totSize = 
		conv1.size() + conv2.size() + conv3.size();

	char* temp = new char[ totSize ];
	char* tbuf = temp;
	conv1.val2buf( tbuf ); tbuf += conv1.size();
	conv2.val2buf( tbuf ); tbuf += conv2.size();
	conv3.val2buf( tbuf ); tbuf += conv3.size();

	MsgFuncBinding mfb( q->mid(), retFunc );
	Qinfo retq( retFunc, e.index(), totSize, 1, !q->isForward() );
	retq.addToQ( Shell::procInfo()->outQid, mfb, temp );
	delete[] temp;
}
