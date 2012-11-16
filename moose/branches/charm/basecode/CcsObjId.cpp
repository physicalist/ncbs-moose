#include "CcsId.h"
#include "CcsObjId.h"
#include "../shell/ShellProxy.h"

using namespace std;

extern ShellProxy *getParserShellProxy();

string CcsObjId::path(string separator) const {
  return getParserShellProxy()->doGetObjIdPath(*this); 
}

const CcsObjId CcsObjId::bad()
{
  static CcsObjId bad_( CcsId(), CcsDataId::bad() );
  return bad_;
}


