#ifndef MOOSE_LOOKUP_HELPER_H
#define MOOSE_LOOKUP_HELPER_H

#include "moose.decl.h"
class Element;
class Id;
class ElementContainer;
#include "../basecode/ThreadId.h"

class LookupHelper : public CBase_LookupHelper {
  vector< Element * > elements_;
  CkVec< ElementContainer * > containers_;
  
  // charm++ entry methods
  public:
  LookupHelper();

  // normal entry methods for manipulating element table
  public:
  Element *get(Id id);
  void set(Id id, Element *e);

  vector< Element * > &elements();
  ThreadId registerContainer(ElementContainer *container);
};

#endif // MOOSE_LOOKUP_HELPER_H
