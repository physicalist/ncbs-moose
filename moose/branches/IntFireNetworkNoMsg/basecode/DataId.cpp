/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <iostream>
using namespace std;
#include "DataId.h"

ostream& operator <<( ostream& s, const DataId& d )
{
	s << d.data_ << ":" << d.field_;
	return s;
}
