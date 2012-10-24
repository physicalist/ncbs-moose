#ifndef MOOSE_LOOKUP_HELPER_H
#define MOOSE_LOOKUP_HELPER_H

#include "moose.decl.h"
class Element;
class Id;
class ElementContainer;
class Shell;
class Clock;
#include "../basecode/ThreadId.h"
class Eref;
#include "../basecode/Id.h"

class LookupHelper : public CBase_LookupHelper {
  vector< Element * > elements_;
  CkVec< ElementContainer * > containers_;

  // save id of and pointer to shell
  Id shellId_;
  Shell *shell_;
  // and pointer to clock, so that it may be queried
  // by element containers
  Clock *clock_;

  char **argv_;
  unsigned int argc_;
  
  // charm++ entry methods
  public:
  LookupHelper(CkVec< string > &s_argv, const CkCallback &cb);
  void initShell(const CkCallback &cb);

  void doSerialUnitTests(const CkCallback &cb);
  void doParallelUnitTests(const CkCallback &cb);

  void iterationDone();
  void invokeFinishCallback();

  // normal methods for manipulating element table
  public:
  Element *get(Id id);
  void set(Id id, Element *e);

  Shell *getShell();
  Clock *getClock();

  vector< Element * > &elements();
  ThreadId registerContainer(ElementContainer *container);
  ElementContainer *getContainer(ThreadId id);
};

#endif // MOOSE_LOOKUP_HELPER_H
