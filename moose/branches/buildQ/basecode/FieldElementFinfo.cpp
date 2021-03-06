/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"

#include "../shell/Shell.h"

bool adopt( Id parent, Id child )
{
	// cout << "PostCreationFunc has called adopt( " << parent()->getName() << ", " << child()->getName() << ")\n";
	return Shell::adopt( parent, child );
}
