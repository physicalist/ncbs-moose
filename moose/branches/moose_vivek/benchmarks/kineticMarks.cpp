/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2014 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include "../shell/Shell.h"

/// Small model, long runtime.
void runKineticsBenchmark1()
{
	Shell* s = reinterpret_cast< Shell* >( ObjId().data() );
	Id mgr = s->doLoadModel( "../Demos/Genesis_files/OSC_Cspace.g", "/model", "Neutral" );
	assert( mgr != Id() );
	s->doReinit();
	s->doStart( 10000.0 );
}

