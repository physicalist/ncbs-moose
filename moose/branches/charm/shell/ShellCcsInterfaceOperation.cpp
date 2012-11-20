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

#define DEF_DESCR_STRING(Class) \
const string Class ## Operation::name_ = #Class;

DEF_DESCR_STRING(SetCwe)
DEF_DESCR_STRING(GetCwe)
DEF_DESCR_STRING(Create)
DEF_DESCR_STRING(Delete)
DEF_DESCR_STRING(AddMsg)
DEF_DESCR_STRING(Quit)
DEF_DESCR_STRING(Start)
DEF_DESCR_STRING(Reinit)
DEF_DESCR_STRING(Stop)
DEF_DESCR_STRING(Terminate)
DEF_DESCR_STRING(Move)
DEF_DESCR_STRING(Copy)
DEF_DESCR_STRING(Find)
DEF_DESCR_STRING(LoadModel)
DEF_DESCR_STRING(UseClock)
DEF_DESCR_STRING(WriteSbml)
DEF_DESCR_STRING(SyncDataHandler)
DEF_DESCR_STRING(ReacDiffMesh)
DEF_DESCR_STRING(SetClock)
DEF_DESCR_STRING(CleanSimulation)
DEF_DESCR_STRING(Adopt)
DEF_DESCR_STRING(GetPath)
DEF_DESCR_STRING(GetObjIdPath)
DEF_DESCR_STRING(GetIsValid)
DEF_DESCR_STRING(Wildcard)
DEF_DESCR_STRING(MsgMgr)


void SetCweOperation::exec(Shell *shell){
  Id *id = CcsPackUnpack<Id>::extractHandler(msg_); 
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
  CcsPackUnpack<CreateStruct>::unpackHandler(msg_, createStruct);
  Id id = shell->doCreate(createStruct.type_,
                          createStruct.parent_,
                          createStruct.name_,
                          createStruct.dimensions_,
                          createStruct.isGlobal_,
                          createStruct.qFlag_);

  CcsSendDelayedReply(delayedReply_, sizeof(Id), &id);
}

void DeleteOperation::exec(Shell *shell){
  std::pair<Id, bool> *pr = CcsPackUnpack<std::pair<Id, bool> >::extractHandler(msg_);
  shell->doDelete(pr->first, pr->second);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void AddMsgOperation::exec(Shell *shell){
  AddMsgStruct addMsgStruct;
  CcsPackUnpack<AddMsgStruct>::unpackHandler(msg_, addMsgStruct);

  MsgId mid = shell->doAddMsg(addMsgStruct.msgType_,
                              addMsgStruct.src_,
                              addMsgStruct.srcField_,
                              addMsgStruct.dest_,
                              addMsgStruct.destField_,
                              addMsgStruct.qFlag_);

  CcsSendDelayedReply(delayedReply_, sizeof(MsgId), &mid);
}

void QuitOperation::exec(Shell *shell){
  bool *arg = CcsPackUnpack<bool>::extractHandler(msg_);
  shell->doQuit(*arg);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void StartOperation::exec(Shell *shell){
  std::pair<double, bool> *arg = CcsPackUnpack<std::pair<double, bool> >::extractHandler(msg_);
  shell->doStart(arg->first, CkCallback(StartOperation::splitPhaseCallback, this), arg->second);
}

bool StartOperation::done(){
  return false;
}

void StartOperation::splitPhaseCallback(void *param, void *msg){
  StartOperation *sop = (StartOperation *) param;
  sop->resume();  
  delete sop;
}

void StartOperation::resume(){
  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void ReinitOperation::exec(Shell *shell){
  bool *arg = CcsPackUnpack<bool>::extractHandler(msg_);
  shell->doReinit(CkCallback(ReinitOperation::splitPhaseCallback, this), *arg);
}

bool ReinitOperation::done(){
  return false;
}

void ReinitOperation::splitPhaseCallback(void *arg, void *msg){
  ReinitOperation *op = (ReinitOperation *) arg;
  op->resume();
  delete op;
}

void ReinitOperation::resume(){
  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void StopOperation::exec(Shell *shell){
  bool *arg = CcsPackUnpack<bool>::extractHandler(msg_);
  shell->doStop(*arg);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void TerminateOperation::exec(Shell *shell){
  bool *arg = CcsPackUnpack<bool>::extractHandler(msg_);
  shell->doQuit(*arg);
#if 0
  shell->doTerminate(*arg);
#endif

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void MoveOperation::exec(Shell *shell){
  MoveStruct *arg = CcsPackUnpack<MoveStruct>::extractHandler(msg_);
  shell->doMove(arg->orig_, arg->newParent_, arg->qFlag_);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void CopyOperation::exec(Shell *shell){
  CopyStruct copyStruct;
  CcsPackUnpack<CopyStruct>::unpackHandler(msg_, copyStruct);

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
  CcsPackUnpack<std::string>::unpackHandler(msg_, path);

  ObjId id = shell->doFind(path);
  CcsSendDelayedReply(delayedReply_, sizeof(ObjId), &id);
}

void UseClockOperation::exec(Shell *shell){
  UseClockStruct use;
  CcsPackUnpack<UseClockStruct>::unpackHandler(msg_, use);

  shell->doUseClock(use.path_,
                    use.field_,
                    use.tick_,
                    use.qFlag_);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void LoadModelOperation::exec(Shell *shell){
  LoadModelStruct load;
  CcsPackUnpack<LoadModelStruct>::unpackHandler(msg_, load);

  Id id = shell->doLoadModel(load.fname_, 
                             load.modelpath_,
                             load.solverClass_,
                             load.qFlag_);

  CcsSendDelayedReply(delayedReply_, sizeof(Id), &id);
}

void WriteSbmlOperation::exec(Shell *shell){
  WriteSbmlStruct write;
  CcsPackUnpack<WriteSbmlStruct>::unpackHandler(msg_, write);

  int ret = shell->doWriteSBML(write.fname_, write.modelpath_, write.qFlag_);

  CcsSendDelayedReply(delayedReply_, sizeof(int), &ret);
}

void SyncDataHandlerOperation::exec(Shell *shell){
  Id *id = CcsPackUnpack<Id>::extractHandler(msg_);
  shell->doSyncDataHandler(*id);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void ReacDiffMeshOperation::exec(Shell *shell){
  CkAbort("ReacDiffMeshOperation not correctly implemented yet\n");
  Id *id = CcsPackUnpack<Id>::extractHandler(msg_);
  shell->doReacDiffMesh(*id);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void SetClockOperation::exec(Shell *shell){
  SetClockStruct *setClock = CcsPackUnpack<SetClockStruct>::extractHandler(msg_);
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
  AdoptStruct *adopt = CcsPackUnpack<AdoptStruct>::extractHandler(msg_);
  shell->adopt(adopt->parent_,
               adopt->child_);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

// can't do this in parser, since we want to keep 
// it separate from moose core
void GetPathOperation::exec(Shell *shell){
  CcsId *ccsId = CcsPackUnpack<CcsId>::extractHandler(msg_);
  string ret = Id(*ccsId).path();

  unsigned int size;
  char *msg = CcsPackUnpack<string>::pack(ret, size);
  CcsSendDelayedReply(delayedReply_, size, msg);

  delete[] msg;
}

void GetObjIdPathOperation::exec(Shell *shell){
  CcsObjId *ccsObjId = CcsPackUnpack<CcsObjId>::extractHandler(msg_);
  string ret = ObjId(*ccsObjId).path();

  unsigned int size;
  char *msg = CcsPackUnpack<string>::pack(ret, size);
  CcsSendDelayedReply(delayedReply_, size, msg);

  delete[] msg;
}

void GetIsValidOperation::exec(Shell *shell){
  CcsId *ccsId = CcsPackUnpack<CcsId>::extractHandler(msg_);
  bool ret = Id::isValid(Id(*ccsId));

  CcsSendDelayedReply(delayedReply_, sizeof(bool), &ret);
}

void WildcardOperation::exec(Shell *shell){
  string path;
  CcsPackUnpack<string>::unpackHandler(msg_, path);
  
  vector<Id> list;
  shell->wildcard(path, list);

  vector<CcsId> toSend;
  for(int i = 0; i < list.size(); i++){
    toSend.push_back(CcsId(list[i].value()));
  }

  unsigned int size;
  char *msg = CcsPackUnpack<vector<CcsId> >::pack(toSend, size);
  CcsSendDelayedReply(delayedReply_, size, msg);

  delete[] msg;
}

void MsgMgrOperation::exec(Shell *shell){
  MsgId *mid = CcsPackUnpack<MsgId>::extractHandler(msg_);

  ObjId oid = Msg::getMsg(*mid)->manager().objId();
  CcsObjId ccsOid(oid.id.value(), oid.dataId.value());
  
  CcsSendDelayedReply(delayedReply_, sizeof(CcsObjId), &ccsOid);
}

