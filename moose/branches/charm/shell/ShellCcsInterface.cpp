#include "converse.h"
#include "ShellCcsInterface.h"
#include "CcsReductionWrapper.h"
#include "ShellCcsInterfaceOperation.h"
#include "../basecode/ThreadId.h"
// need these includes for Shell
#include "header.h"
#include "SingleMsg.h"
#include "DiagonalMsg.h"
#include "OneToOneMsg.h"
#include "OneToAllMsg.h"
#include "SparseMatrix.h"
#include "SparseMsg.h"
#include "ReduceMsg.h"
#include "ReduceFinfo.h"
#include "Shell.h"
#include "Dinfo.h"
#include "Wildcard.h"

#include "ShellProxy.h"
#include "Shell.h"
#include "../charm/LookupHelper.h"

#define SHELL_CCS_INTERFACE_VERBOSE CkPrintf

extern CProxy_ShellCcsInterface readonlyShellCcsInterfaceProxy;
extern CProxy_LookupHelper readonlyLookupHelperProxy;

ShellCcsInterface::ShellCcsInterface(const CkCallback &cb){
  // register handlers
  CcsRegisterHandler(ShellProxy::setCweHandlerString, (CmiHandler) ShellCcsInterface::setCwe);
  CcsRegisterHandler(ShellProxy::getCweHandlerString, (CmiHandler) ShellCcsInterface::getCwe);
  CcsRegisterHandler(ShellProxy::doCreateHandlerString, (CmiHandler) ShellCcsInterface::doCreate);
  CcsRegisterHandler(ShellProxy::doDeleteHandlerString, (CmiHandler) ShellCcsInterface::doDelete);
  CcsRegisterHandler(ShellProxy::doAddMsgHandlerString, (CmiHandler) ShellCcsInterface::doAddMsg);
  CcsRegisterHandler(ShellProxy::doQuitHandlerString, (CmiHandler) ShellCcsInterface::doQuit);
  CcsRegisterHandler(ShellProxy::doStartHandlerString, (CmiHandler) ShellCcsInterface::doStart);
  CcsRegisterHandler(ShellProxy::doReinitHandlerString, (CmiHandler) ShellCcsInterface::doReinit);
  CcsRegisterHandler(ShellProxy::doStopHandlerString, (CmiHandler) ShellCcsInterface::doStop);
  CcsRegisterHandler(ShellProxy::doTerminateHandlerString, (CmiHandler) ShellCcsInterface::doTerminate);
  CcsRegisterHandler(ShellProxy::doMoveHandlerString, (CmiHandler) ShellCcsInterface::doMove);
  CcsRegisterHandler(ShellProxy::doCopyHandlerString, (CmiHandler) ShellCcsInterface::doCopy);
  CcsRegisterHandler(ShellProxy::doFindHandlerString, (CmiHandler) ShellCcsInterface::doFind);
  CcsRegisterHandler(ShellProxy::doUseClockHandlerString, (CmiHandler) ShellCcsInterface::doUseClock);
  CcsRegisterHandler(ShellProxy::doLoadModelHandlerString, (CmiHandler) ShellCcsInterface::doLoadModel);
  CcsRegisterHandler(ShellProxy::doWriteSBMLHandlerString, (CmiHandler) ShellCcsInterface::doWriteSBML);
  CcsRegisterHandler(ShellProxy::doSyncDataHandlerString, (CmiHandler) ShellCcsInterface::doSyncDataHandler);
  CcsRegisterHandler(ShellProxy::doReacDiffMeshHandlerString, (CmiHandler) ShellCcsInterface::doReacDiffMesh);
  CcsRegisterHandler(ShellProxy::doSetClockHandlerString, (CmiHandler) ShellCcsInterface::doSetClock);
  CcsRegisterHandler(ShellProxy::doCleanSimulationHandlerString, (CmiHandler) ShellCcsInterface::doCleanSimulation);
  CcsRegisterHandler(ShellProxy::doAdoptHandlerString, (CmiHandler) ShellCcsInterface::doAdopt);


  CcsRegisterHandler(ShellProxy::doGetPathHandlerString, (CmiHandler) ShellCcsInterface::doGetPath);
  CcsRegisterHandler(ShellProxy::doGetObjIdPathHandlerString, (CmiHandler) ShellCcsInterface::doGetObjIdPath);
  CcsRegisterHandler(ShellProxy::doGetIsValidHandlerString, (CmiHandler) ShellCcsInterface::doGetIsValid);
  CcsRegisterHandler(ShellProxy::doWildcardHandlerString, (CmiHandler) ShellCcsInterface::doWildcard);
  CcsRegisterHandler(ShellProxy::doGetMsgMgrHandlerString, (CmiHandler) ShellCcsInterface::doGetMsgMgr);

  // register reducer merge functions
  CcsSetMergeFn(ShellProxy::setCweHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doCreateHandlerString, CmiReduceMergeFn_random);
  CcsSetMergeFn(ShellProxy::doDeleteHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doAddMsgHandlerString, CmiReduceMergeFn_random);
  CcsSetMergeFn(ShellProxy::doQuitHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doStartHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doReinitHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doStopHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doTerminateHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doMoveHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doCopyHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doFindHandlerString, CmiReduceMergeFn_random);
  CcsSetMergeFn(ShellProxy::doUseClockHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doLoadModelHandlerString, CmiReduceMergeFn_random);
  CcsSetMergeFn(ShellProxy::doWriteSBMLHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doSyncDataHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doReacDiffMeshHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doSetClockHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doCleanSimulationHandlerString, CcsMerge_logical_and);
  CcsSetMergeFn(ShellProxy::doAdoptHandlerString, CcsMerge_logical_and);


  contribute(cb);
}

// static CCS handlers

template<typename OP_TYPE>
void ShellCcsInterface::process(char *msg){
  Shell *shell = getLocalShell();
  ShellCcsInterface *iface = getLocalShellCcsInterface();
  iface->addPendingOp(new OP_TYPE(msg, CcsDelayReply()));
  if(!shell->isRunning()){
    readonlyShellCcsInterfaceProxy.ckLocalBranch()->poll(shell);
  }
}

void ShellCcsInterface::poll(Shell *shell){
  CkAssert(!shell->isRunning());
  if(opQ.empty()) return;

  ShellCcsInterfaceOperation *op = opQ.front();
  opQ.pop();

  SHELL_CCS_INTERFACE_VERBOSE("[%d] %s::exec\n", CkMyPe(), op->getName().c_str());
  op->exec(shell);
  
  if(op->done()) delete op;
}

void ShellCcsInterface::addPendingOp(ShellCcsInterfaceOperation *op){
  opQ.push(op); 
}

void ShellCcsInterface::setCwe(char *msg){
  process<SetCweOperation>(msg);
}

void ShellCcsInterface::getCwe(char *msg){
  process<SetCweOperation>(msg);
}

void ShellCcsInterface::doCreate(char *msg){
  process<CreateOperation>(msg);
}

void ShellCcsInterface::doDelete(char *msg){
  process<DeleteOperation>(msg);
}

void ShellCcsInterface::doAddMsg(char *msg){
  process<AddMsgOperation>(msg);
}

void ShellCcsInterface::doQuit(char *msg){
  process<QuitOperation>(msg);
}

void ShellCcsInterface::doStart(char *msg){
  process<StartOperation>(msg);
}

void ShellCcsInterface::doReinit(char *msg){
  process<ReinitOperation>(msg);
}

void ShellCcsInterface::doStop(char *msg){
  process<StopOperation>(msg);
}

void ShellCcsInterface::doTerminate(char *msg){
  process<TerminateOperation>(msg);
}

void ShellCcsInterface::doMove(char *msg){
  process<MoveOperation>(msg);
}

void ShellCcsInterface::doCopy(char *msg){
  process<CopyOperation>(msg);
}

void ShellCcsInterface::doFind(char *msg){
  process<FindOperation>(msg);
}

void ShellCcsInterface::doUseClock(char *msg){
  process<UseClockOperation>(msg);
}

void ShellCcsInterface::doLoadModel(char *msg){
  process<LoadModelOperation>(msg);
}

void ShellCcsInterface::doWriteSBML(char *msg){
  process<WriteSbmlOperation>(msg);
}

void ShellCcsInterface::doSyncDataHandler(char *msg){
  process<SyncDataHandlerOperation>(msg);
}

void ShellCcsInterface::doReacDiffMesh(char *msg){
  process<ReacDiffMeshOperation>(msg);
}

void ShellCcsInterface::doSetClock(char *msg){
  process<SetClockOperation>(msg);
}

void ShellCcsInterface::doCleanSimulation(char *msg){
  process<CleanSimulationOperation>(msg);
}

void ShellCcsInterface::doAdopt(char *msg){
  process<AdoptOperation>(msg);
}

void ShellCcsInterface::doGetPath(char *msg){
  process<GetPathOperation>(msg);
}

void ShellCcsInterface::doGetObjIdPath(char *msg){
  process<GetObjIdPathOperation>(msg);
}

void ShellCcsInterface::doGetIsValid(char *msg){
  process<GetIsValidOperation>(msg);
}

void ShellCcsInterface::doWildcard(char *msg){
  process<WildcardOperation>(msg);
}

void ShellCcsInterface::doGetMsgMgr(char *msg){
  process<MsgMgrOperation>(msg);
}

Shell *ShellCcsInterface::getLocalShell(){
  return (Shell *) readonlyLookupHelperProxy.ckLocalBranch()->getShell();
}

ShellCcsInterface *ShellCcsInterface::getLocalShellCcsInterface(){
  return readonlyShellCcsInterfaceProxy.ckLocalBranch();
}




