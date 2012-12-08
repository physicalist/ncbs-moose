#include "CcsId.h"
#include "CcsObjId.h"
#include "../shell/ShellProxy.h"

using namespace std;

std::ostream &operator<<(std::ostream &out, CcsObjId &oid){
  out << "[" << oid.id.value() << "," << oid.dataId.value() << "]";
  return out;
}
