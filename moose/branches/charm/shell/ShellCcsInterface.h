#ifndef MOOSE_SHELL_CCS_INTERFACE_H
#define MOOSE_SHELL_CCS_INTERFACE_H

#include <string>
#include <queue>
using namespace std;

#include "converse.h"
#include "../charm/moose.decl.h"


class Shell;
class ShellCcsInterfaceOperation;

// this will be a charm++ group
class ShellCcsInterface : public CBase_ShellCcsInterface {
  // object data members
  private:
  // if the shell is busy doing something (either 
  // running the simulation, or modifying its structure)
  // when it receives a command from the user, the 
  // command is placed in this queue
  std::queue<ShellCcsInterfaceOperation *> opQ;

  // charm++ interface
  public:

  ShellCcsInterface(const CkCallback &cb);

  public:
  // called by (static) CCS handler functions to 
  // place operations in pending operation queue
  void addPendingOp(ShellCcsInterfaceOperation *op);
  // to examine front of buffered op queue

  // called by Shell when it finishes running,
  // in order to get further instructions
  void poll(Shell *shell);

  // the following are the message handlers for
  // different CCS actions
  public:

  static void setCwe(char *msg);
  static void getCwe(char *msg);
  static void doCreate(char *msg);
  static void doDelete(char *msg);
  static void doAddMsg(char *msg);
  static void doQuit(char *msg);
  static void doStart(char *msg);
  static void doReinit(char *msg);
  static void doStop(char *msg);
  static void doTerminate(char *msg);
  static void doMove(char *msg);
  static void doCopy(char *msg);
  static void doFind(char *msg);
  static void doUseClock(char *msg);
  static void doLoadModel(char *msg);
  static void doWriteSBML(char *msg);
  static void doSyncDataHandler(char *msg);
  static void doReacDiffMesh(char *msg);
  static void doSetClock(char *msg);
  static void doCleanSimulation(char *msg);
  static void doAdopt(char *msg);

  static void doGetPath(char *msg);
  static void doGetObjIdPath(char *msg);
  static void doGetIsValid(char *msg);
  static void doWildcard(char *msg);
  static void doGetMsgMgr(char *msg);
  static void doGetCinfos(char *msg);

  private:
  // for the benefit of the CCS interface functions
  static Shell *getLocalShell();
  static ShellCcsInterface *getLocalShellCcsInterface();

  public:
  // We have a single merger function, which works
  // with types of fixed size. This merger function is
  // templated on the WRAPPER_TYPE, which encapsulates
  // the data type to be reduced, and does the actual
  // reduction of data. This is done by either calling 
  // reduce() on data items, or for simple data types, 
  // using specializations.
  template<typename WRAPPER_TYPE>
  static void *reductionMerger(int *localSize, void *localContribution, 
                               void **remoteContributions, int nRemoteContributions); 

  // utility function that creates and enqueues the 
  // correct type of ShellCcsInterfaceOperation based
  // on the handler invoked via CCS. If the Shell is
  // not running something at the time of enqueuing, 
  // we can pass the instruction to it. Otherwise, we
  // must wait until the Shell poll()s the 
  // ShellCcsInterface for an instruction.
  // The actual unpacking
  // of data and its delivery to the Shell is done by 
  // the particular ShellCcsInterfaceOperation.
  template<typename OP_TYPE>
  static void process(char *msg);

};

#endif // MOOSE_SHELL_CCS_INTERFACE_H
