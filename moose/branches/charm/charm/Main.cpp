#include <string>
#include "SimulationParameters.h"
#include "moose.decl.h"
#include "ElementContainer.h"
#include "Main.h"

// readonly variables
CProxy_ElementContainer readonlyElementContainerProxy;
CProxy_Main readonlyMainProxy;

SimulationParameters readonlySimulationParameters;

// externally defined MOOSE functions
extern Id init( int argc, char** argv, bool& doUnitTests, bool& doRegressionTests );

Main::Main(CkArgMsg *m){
  __sdag_init();
  doUnitTests_ = false;
  createMooseParallelObjects();
  thisProxy.waitUntilInitDone();
}

void Main::createMooseParallelObjects(){
  // XXX should get this from command line parameters
  nElementContainers_ = CkNumPes();
  __sdag_init();

  CkCallback cb(CkIndex_Main::elementsReady(), thisProxy);
  readonlyElementContainerProxy = CProxy_ElementContainer::ckNew(cb, nElementContainers_);
  readonlyMainProxy = thisProxy;

}

void Main::commence(){

  CkPrintf("[main] register containers with shells\n");
  readonlyElementContainerProxy.registerWithShell(CkCallbackResumeThread());

  CkPrintf("[main] starting serial unit tests\n");
  readonlyElementContainerProxy.doSerialUnitTests(CkCallbackResumeThread());

  // XXX - only for the time being
  // once we have the unit tests running, we should 
  // incorporate the next steps, below
  CkExit();

  // at this point, we have initialized the shells and
  // the shell ccs interfaces, and have created the 
  // element container chares.
  // moreover, the CCS server has started up 
  // XXX Therefore, we
  // should send the initiator a message, telling it to
  // launch the parser and initiate a CCS session 
}

#include "moose.def.h"
