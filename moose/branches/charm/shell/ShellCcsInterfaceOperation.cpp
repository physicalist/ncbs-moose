#include "ShellCcsInterfaceOperation.h"
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
#include "Shell.h"
#include "converse.h"

#include "ShellProxyHelpers.h"
#include "CcsPackUnpack.h"

void SetCweOperation::exec(Shell *shell){
  Id *id = CcsPackUnpack<Id>::cast(msg_); 
  shell->setCwe(*id);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void GetCweOperation::exec(Shell *shell){
  Id id = shell->getCwe(); 
  CcsSendDelayedReply(delayedReply_, sizeof(Id), &id);
}

void CreateOperation::exec(Shell *shell){
  CreateStruct createStruct;
  CcsPackUnpack<CreateStruct>::unpack(msg_, createStruct);
  Id id = shell->doCreate(createStruct.type_,
                          createStruct.parent_,
                          createStruct.name_,
                          createStruct.dimensions_,
                          createStruct.isGlobal_,
                          createStruct.qFlag_);

  CcsSendDelayedReply(delayedReply_, sizeof(Id), &id);
}

void DeleteOperation::exec(Shell *shell){
  std::pair<Id, bool> *pr = CcsPackUnpack<std::pair<Id, bool> >::cast(msg_);
  shell->doDelete(pr->first, pr->second);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void AddMsgOperation::exec(Shell *shell){
  AddMsgStruct addMsgStruct;
  CcsPackUnpack<AddMsgStruct>::unpack(msg_, addMsgStruct);

  MsgId mid = shell->doAddMsg(addMsgStruct.msgType_,
                              addMsgStruct.src_,
                              addMsgStruct.srcField_,
                              addMsgStruct.dest_,
                              addMsgStruct.destField_,
                              addMsgStruct.qFlag_);

  CcsSendDelayedReply(delayedReply_, sizeof(MsgId), &mid);
}

void QuitOperation::exec(Shell *shell){
  bool *arg = CcsPackUnpack<bool>::cast(msg_);
  shell->doQuit(*arg);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void StartOperation::exec(Shell *shell){
  std::pair<double, bool> *arg = CcsPackUnpack<std::pair<double, bool> >::cast(msg_);
  shell->doStart(arg->first, CkCallback(StartOperation::splitPhaseCallback, this), arg->second);
}

void StartOperation::splitPhaseCallback(void *param, void *msg){
  StartOperation *sop = (StartOperation *) param;
  sop->resume();  
}

void StartOperation::resume(){
  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void ReinitOperation::exec(Shell *shell){
  bool *arg = CcsPackUnpack<bool>::cast(msg_);
  shell->doReinit(CkCallback(ReinitOperation::splitPhaseCallback, this), *arg);
}

void ReinitOperation::splitPhaseCallback(void *arg, void *msg){
  ReinitOperation *op = (ReinitOperation *) arg;
  op->resume();
}

void ReinitOperation::resume(){
  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void StopOperation::exec(Shell *shell){
  bool *arg = CcsPackUnpack<bool>::cast(msg_);
  shell->doStop(*arg);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void TerminateOperation::exec(Shell *shell){
  bool *arg = CcsPackUnpack<bool>::cast(msg_);
  shell->doQuit(*arg);
#if 0
  shell->doTerminate(*arg);
#endif

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void MoveOperation::exec(Shell *shell){
  MoveStruct *arg = CcsPackUnpack<MoveStruct>::cast(msg_);
  shell->doMove(arg->orig_, arg->newParent_, arg->qFlag_);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void CopyOperation::exec(Shell *shell){
  CopyStruct copyStruct;
  CcsPackUnpack<CopyStruct>::unpack(msg_, copyStruct);

  Id id = shell->doCopy(copyStruct.orig_,
                        copyStruct.newParent_,
                        copyStruct.newName_,
                        copyStruct.n_,
                        copyStruct.toGlobal_,
                        copyStruct.copyExtMsgs_,
                        copyStruct.qFlag_);

  CcsSendDelayedReply(delayedReply_, sizeof(Id), &id);
}

void FindOperation::exec(Shell *shell){
  std::string path;
  CcsPackUnpack<std::string>::unpack(msg_, path);

  ObjId id = shell->doFind(path);
  CcsSendDelayedReply(delayedReply_, sizeof(ObjId), &id);
}

void UseClockOperation::exec(Shell *shell){
  UseClockStruct use;
  CcsPackUnpack<UseClockStruct>::unpack(msg_, use);

  shell->doUseClock(use.path_,
                    use.field_,
                    use.tick_,
                    use.qFlag_);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void LoadModelOperation::exec(Shell *shell){
  LoadModelStruct load;
  CcsPackUnpack<LoadModelStruct>::unpack(msg_, load);

  Id id = shell->doLoadModel(load.fname_, 
                             load.modelpath_,
                             load.solverClass_,
                             load.qFlag_);

  CcsSendDelayedReply(delayedReply_, sizeof(Id), &id);
}

void WriteSbmlOperation::exec(Shell *shell){
  WriteSbmlStruct write;
  CcsPackUnpack<WriteSbmlStruct>::unpack(msg_, write);

  int ret = shell->doWriteSBML(write.fname_, write.modelpath_, write.qFlag_);

  CcsSendDelayedReply(delayedReply_, sizeof(int), &ret);
}

void SyncDataHandlerOperation::exec(Shell *shell){
  Id *id = CcsPackUnpack<Id>::cast(msg_);
  shell->doSyncDataHandler(*id);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void ReacDiffMeshOperation::exec(Shell *shell){
  CkAbort("ReacDiffMeshOperation not correctly implemented yet\n");
  Id *id = CcsPackUnpack<Id>::cast(msg_);
  shell->doReacDiffMesh(*id);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void SetClockOperation::exec(Shell *shell){
  SetClockStruct *setClock = CcsPackUnpack<SetClockStruct>::cast(msg_);
  shell->doSetClock(setClock->tickNum_,
                    setClock->dt_,
                    setClock->qFlag_);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void CleanSimulationOperation::exec(Shell *shell){
  shell->cleanSimulation();

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void AdoptOperation::exec(Shell *shell){
  AdoptStruct *adopt = CcsPackUnpack<AdoptStruct>::cast(msg_);
  shell->adopt(adopt->parent_,
               adopt->child_);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}


