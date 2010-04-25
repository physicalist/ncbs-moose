/**********************************************************************
 ** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "Message.h"

OneToAllMsg::OneToAllMsg( Eref e1, Element* e2 )
	: 
		Msg( e1.element(), e2 ),
		i1_( e1.index() )
{
	;
}

/**
 * Need to revisit to handle nodes
 */
void OneToAllMsg::exec( const char* arg, const ProcInfo *p ) const
{
	const Qinfo *q = ( reinterpret_cast < const Qinfo * >( arg ) );
	// arg += sizeof( Qinfo );
	if ( q->isForward() ) {
		if ( q->srcIndex() == i1_ ) {
		const OpFunc* f = e2_->cinfo()->getOpFunc( q->fid() );
		if ( e2_->dataHandler()->numDimensions() == 1 ) {
			for ( unsigned int i = 0; i < e2_->dataHandler()->numData(); ++i )
				f->op( Eref( e2_, i ), arg );
		} else if ( e2_->dataHandler()->numDimensions() == 2 ) {
			for ( unsigned int i = 0; i < e2_->dataHandler()->numData1(); ++i )
				for ( unsigned int j = 0; j < e2_->dataHandler()->numData2( i ); ++j )
					f->op( Eref( e2_, DataId( i, j ) ), arg );
		}
		}
	} else {
		const OpFunc* f = e1_->cinfo()->getOpFunc( q->fid() );
		f->op( Eref( e1_, i1_ ), arg );
	}
}

bool OneToAllMsg::add( Eref e1, const string& srcField, 
			Element* e2, const string& destField )
{
	FuncId funcId;
	const SrcFinfo* srcFinfo = validateMsg( e1.element(), srcField,
		e2, destField, funcId );

	if ( srcFinfo ) {
		Msg* m = new OneToAllMsg( e1, e2 );
		e1.element()->addMsgAndFunc( m->mid(), funcId, srcFinfo->getBindIndex() );
		return 1;
	}
	return 0;
}

