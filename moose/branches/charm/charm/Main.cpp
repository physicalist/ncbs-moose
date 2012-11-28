#include <string>
#include <iostream>
using namespace std;

#include "pup_stl.h"

#include "../basecode/ProcInfo.h"
class Element;
#include "../basecode/DataId.h"
class Eref;
#include "../basecode/Id.h"
#include "../basecode/ObjId.h"
#include "../basecode/Eref.h"
#include "../basecode/BindIndex.h"
#include "../basecode/FuncId.h"
#include "../basecode/ThreadId.h"
#include "../basecode/Qinfo.h"

class Cinfo;
#include "../basecode/MsgId.h"
#include "../basecode/Finfo.h"
class OpFunc;
#include "../basecode/Cinfo.h"
#include "../scheduling/Tick.h"
#include "../scheduling/TickMgr.h"
#include "../scheduling/TickPtr.h"
#include "../scheduling/Clock.h"
class DataHandler;
struct DimInfo;
class MsgFuncBinding;
class DestFinfo;
#include "../basecode/Element.h"
#include "LookupHelper.h"
#include "../shell/ShellCcsInterface.h" 
#include "SimulationParameters.h"
#include "moose.decl.h"
#include "ElementContainer.h"
#include "Main.h"
#include "ParallelTestHelper.h"




// readonly variables
unsigned int readonlyNumElementContainers;
unsigned int readonlyNumUsefulElementContainers;
CProxy_ElementContainer readonlyElementContainerProxy;
CProxy_LookupHelper readonlyLookupHelperProxy;
CProxy_ShellCcsInterface readonlyShellCcsInterfaceProxy;
CProxy_Main readonlyMainProxy;

extern CProxy_ParallelTestHelper readonlyParallelTestHelperProxy;

SimulationParameters readonlySimulationParameters;

// externally defined MOOSE functions
extern Id init( int argc, char** argv, bool& doUnitTests, bool& doRegressionTests );

Main::Main(CkArgMsg *m){
  __sdag_init();
  doUnitTests_ = false;
  createMooseParallelObjects(m);
  thisProxy.waitUntilInitDone();
}

void Main::createMooseParallelObjects(CkArgMsg *m){
  // XXX should get this from command line parameters
  // one "shell/script/test" container on each PE is required in addition to
  // at least one compute container
  readonlyNumElementContainers = 2 * CkNumPes();
  readonlyNumUsefulElementContainers = CkNumPes();

  __sdag_init();

  CkVec< string > args;
  for(int i = 0; i < m->argc; i++) args.push_back(m->argv[i]);

  // initiate creation of lookup helper group
  readonlyLookupHelperProxy = CProxy_LookupHelper::ckNew(args, CkCallback(CkIndex_Main::lookupHelpersReady(), thisProxy));

  // initiate creation of Shell-CCS interface group
  readonlyShellCcsInterfaceProxy = CProxy_ShellCcsInterface::ckNew(CkCallback(CkIndex_Main::shellCcsInterfacesReady(), thisProxy));

  // initiate creation of chare array elements
  readonlyElementContainerProxy = CProxy_ElementContainer::ckNew(CkCallback(CkIndex_Main::elementsReady(), thisProxy), readonlyNumElementContainers);

  readonlyParallelTestHelperProxy = CProxy_ParallelTestHelper::ckNew();

  readonlyMainProxy = thisProxy;

}

void Main::commence(){
  CkPrintf("[main] shell init...");
  readonlyLookupHelperProxy.initShell(CkCallbackResumeThread());
  CkPrintf("done\n");

  CkPrintf("[main] register containers with helpers...");
  readonlyElementContainerProxy.registerSelf(CkCallbackResumeThread());
  CkPrintf("done\n");

  CkPrintf("[main] serial unit tests...");
  readonlyParallelTestHelperProxy[0].doSerialUnitTests(CkCallbackResumeThread());
  CkPrintf("done\n");

  CkPrintf("[main] ready for CCS commands\n");

  //CkPrintf("\n[main] starting parallel unit tests\n");
  //readonlyParallelTestHelperProxy[0].doMpiTests(CkCallbackResumeThread());

  //CkPrintf("\n[main] starting process unit tests\n");
  //readonlyParallelTestHelperProxy[0].doProcessTests(CkCallbackResumeThread());

  //CkPrintf("\n[main] starting regression tests\n");
  //readonlyParallelTestHelperProxy[0].doRegressionTests(CkCallbackResumeThread());

  // XXX disabled all parallel unit tests, since many of them make improper assumptions and assertions
  // Moreover, shell commands must be sent to all nodes, not just node 0, and in a synchronized manner.
  // So, we must first interface with the parser using CCS, and then do testing

  // XXX - only for the time being
  // once we have the unit tests running, we should 
  // incorporate the next steps, below
  //CkExit();

  // at this point, we have initialized the shells and
  // the shell ccs interfaces, and have created the 
  // element container chares.
  // moreover, the CCS server has started up 
  // XXX Therefore, we
  // should send the initiator a message, telling it to
  // launch the parser and initiate a CCS session 
}

extern void ccsRegistration();
#include "moose.def.h"
