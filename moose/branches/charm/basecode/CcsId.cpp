#include "CcsId.h"
#include "CcsObjId.h"
#include "../shell/ShellProxy.h"

using namespace std;

#ifdef PYMOOSE
extern ShellProxy *getParserShellProxy();

CcsId::CcsId( std::string path, std::string separator){
  id_ = getParserShellProxy()->doFind(path).id.id_; 
}

string CcsId::path(string separator) const {
  return getParserShellProxy()->doGetPath(*this); 
}

bool CcsId::isValid(CcsId id){
  return getParserShellProxy()->doGetIsValid(id);
}
#endif

std::ostream &operator<<(std::ostream &out, CcsId &id){
  out << id.value();
  return out;
}
