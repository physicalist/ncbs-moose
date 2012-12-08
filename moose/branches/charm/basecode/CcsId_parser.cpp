#include "CcsId.h"
#include "CcsObjId.h"
#include "../shell/ShellProxy.h"

// shouldn't link this code into the moose-core,
// since there is no definition of getParserShellProxy
// therein. this defn. is given by either pymoose 
// or charm_test 

extern "C" ShellProxy *getParserShellProxy();

CcsId::CcsId( std::string path, std::string separator){
  id_ = getParserShellProxy()->doFind(path).id.id_; 
}

string CcsId::path(string separator) const {
  return getParserShellProxy()->doGetPath(*this); 
}

bool CcsId::isValid(CcsId id){
  return getParserShellProxy()->doGetIsValid(id);
}


