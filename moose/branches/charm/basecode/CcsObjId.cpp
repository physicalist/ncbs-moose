#include "CcsId.h"
#include "CcsObjId.h"
#include "../shell/ShellProxy.h"

using namespace std;

#ifdef PYMOOSE
// need these only for pymoose
// defined in pymoose/moosemodule.cpp
extern ShellProxy *getParserShellProxy();

string CcsObjId::path(string separator) const {
  return getParserShellProxy()->doGetObjIdPath(*this); 
}

const CcsObjId CcsObjId::bad()
{
  static CcsObjId bad_( CcsId(), CcsDataId::bad() );
  return bad_;
}
#endif

std::ostream &operator<<(std::ostream &out, CcsObjId &oid){
  out << "[" << oid.id.value() << "," << oid.dataId.value() << "]";
  return out;
}
