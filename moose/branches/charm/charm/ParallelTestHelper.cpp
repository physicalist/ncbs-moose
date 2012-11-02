#include "ParallelTestHelper.h"
extern void testAsync();
extern void testMsg();
extern void testShell();
extern void testScheduling();
extern void testBuiltins();
extern void testKinetics();
extern void testBiophysics();
extern void testHSolve();
extern void testGeom();
extern void testMesh();

extern void testMpiMsg();
extern void testMpiShell();
extern void testMpiBuiltins();
extern void testMpiScheduling();

#ifdef USE_SMOLDYN
extern void testSmoldyn();
#endif



extern void mpiTests();
class Shell;
extern void processTests(Shell *shell);

CProxy_ParallelTestHelper readonlyParallelTestHelperProxy;

void ParallelTestHelper::doSerialUnitTests(const CkCallback &cb){
#if 0
  testAsync();
  testMsg();
  testShell();
  testScheduling();
  testBuiltins();
  testKinetics();
  testBiophysics();
  testHSolve();
  testGeom();
  testMesh();
#ifdef USE_SMOLDYN
  testSmoldyn();
#endif
#endif
  cb.send();
}

void ParallelTestHelper::doMpiTests(const CkCallback &cb){
  mpiTests();
  contribute(cb);
}

void ParallelTestHelper::doProcessTests(const CkCallback &cb){
  processTests(NULL);
  contribute(cb);
}


