#include <string>
#include <vector>

using namespace std;

#include "LookupHelper.h"
class Eref;
#include "../basecode/Id.h"

CProxy_LookupHelper readonlyLookupHelperProxy;

LookupHelper::LookupHelper(){
}

Element *LookupHelper::get(Id id){
  return elements_[id.value()];
}

void LookupHelper::set(Id id, Element *e){
  elements_[id.value()] = e;
}

vector< Element * > &LookupHelper::elements(){
  return elements_;
}

ThreadId LookupHelper::registerContainer(ElementContainer *container){
  ThreadId index = containers_.size();
  containers_.push_back(container);
  return index;
}
