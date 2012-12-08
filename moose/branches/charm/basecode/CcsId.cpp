#include "CcsId.h"

using namespace std;

std::ostream &operator<<(std::ostream &out, CcsId &id){
  out << id.value();
  return out;
}


