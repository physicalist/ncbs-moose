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
  CcsSetMergeFn(ShellProxy::setCweHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::getCweHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<CcsId> >);
  CcsSetMergeFn(ShellProxy::doCreateHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<CcsId> >);
  CcsSetMergeFn(ShellProxy::doDeleteHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doAddMsgHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<MsgId> >);
  CcsSetMergeFn(ShellProxy::doQuitHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doStartHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<CcsId> >);
  CcsSetMergeFn(ShellProxy::doReinitHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doStopHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doTerminateHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doMoveHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doCopyHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doFindHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<CcsObjId> >);
  CcsSetMergeFn(ShellProxy::doUseClockHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doLoadModelHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<CcsId> >);
  CcsSetMergeFn(ShellProxy::doWriteSBMLHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<int> >);
  CcsSetMergeFn(ShellProxy::doSyncDataHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doReacDiffMeshHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doSetClockHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doCleanSimulationHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);
  CcsSetMergeFn(ShellProxy::doAdoptHandlerString, ShellCcsInterface::reductionMerger<CcsReductionWrapper<bool> >);


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
  op->exec(shell);

  opQ.pop();
  delete op;
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



// CCS REDUCTION MERGER FUNCTION

template<typename WRAPPER_TYPE>
void *ShellCcsInterface::reductionMerger(int *localSize, void *localContribution, void **remoteContributions, int nRemoteContributions){
  CcsImplHeader *header = (CcsImplHeader *) (((char *) localContribution) + CmiReservedHeaderSize);

  // check whether local contribution is sane          
  CmiAssert(ChMessageInt(header->len) == sizeof(WRAPPER_TYPE));  

  int msgSize = CmiReservedHeaderSize + sizeof(CcsImplHeader) + ChMessageInt(header->len);
  CmiAssert(*localSize == msgSize);

  // this is the locally contributed data
  WRAPPER_TYPE *localData = (WRAPPER_TYPE *)(header + 1);

  // go through recvd remote contributions and accumulate
  for(int i = 0; i < nRemoteContributions; i++){       
    // check whether remote contributions are sane     
    header = (CcsImplHeader *)(((char *) remoteContributions[i]) + CmiReservedHeaderSize);
    CmiAssert(ChMessageInt(header->len) == sizeof(WRAPPER_TYPE));

    // accumulate remote contribution 
    WRAPPER_TYPE *remoteData = (WRAPPER_TYPE *)(header + 1);     
    *localData += *remoteData;
  }

  // reuse the local message
  void *replyMsg = localContribution;
  header = (CcsImplHeader *)(((char *)replyMsg) + CmiReservedHeaderSize);
  header->len = ChMessageInt_new(sizeof(WRAPPER_TYPE));

  return replyMsg;
}


