#ifndef MOOSE_TEST_SHELL_H
#define MOOSE_TEST_SHELL_H

#include <string>
using namespace std;
#include "../charm/moose.decl.h"

// this class is needed because we want to use 
// ckcallbackresumethread in the test.
// for this purpose we need a 
// charm++ object with a [threaded] entry method
class ParallelTestHelper : public CBase_ParallelTestHelper { 
  public:
  ParallelTestHelper() {}
  // this is the threaded entry method
  void doSerialUnitTests(const CkCallback &cb);
  void doMpiTests(const CkCallback &cb);
  void doProcessTests(const CkCallback &cb);
  void doRegressionTests(const CkCallback &cb);
};
#endif
