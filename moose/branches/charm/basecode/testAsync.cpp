/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "TestAsync.h"
#include "../charm/ElementContainer.h"

SrcFinfo0 Test::s0( "s0", "");
Finfo* Test::sharedVec[6];

void ElementContainer::doSerialUnitTests(const CkCallback &cb){
  TestAsync basecode(this);
  basecode.testAsync();

  contribute(cb);
}
