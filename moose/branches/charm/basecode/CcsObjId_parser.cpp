#include "CcsId.h"
#include "CcsObjId.h"
#include "../shell/ShellProxy.h"

// need these only for pymoose
// defined in pymoose/moosemodule.cpp
// and a dummy version exists in charm_test/Main.cpp
// this is ok because charm_test is completely diff from pymoose
// moreover, this code shouldn't be linked into the moose
// exec, since there is no definition of getParserShellProxy() therein:
// this function is defined either by pymoose or the 
// charm++ test driver, charm_test
extern "C" ShellProxy *getParserShellProxy();

string CcsObjId::path(string separator) const {
  return getParserShellProxy()->doGetObjIdPath(*this); 
}

const CcsObjId CcsObjId::bad()
{
  static CcsObjId bad_( CcsId(), CcsDataId::bad() );
  return bad_;
}


