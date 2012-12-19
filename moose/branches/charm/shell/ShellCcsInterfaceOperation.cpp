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

#include "../basecode/SetGetWrapper.h"
#include "ShellProxyHelpers.h"
#include "../basecode/CcsPackUnpack.h"

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
DEF_DESCR_STRING(Cinfos)


void SetCweOperation::exec(Shell *shell){
  Id *id = CcsPackUnpack<Id>::extractHandler(msg_); 
  shell->setCwe(*id);

  bool r = true;
  CcsSendDelayedReply(delayedReply_, sizeof(bool), &r);
}

void GetCweOperation::exec(Shell *shell){
  Id id = shell->getCwe(); 

  CcsId ccsId(id.value());
  CcsSendDelayedReply(delayedReply_, sizeof(CcsId), &ccsId);
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

  CcsId ccsId(id.value());

  CcsSendDelayedReply(delayedReply_, sizeof(CcsId), &ccsId);
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

// this will cause an 'unclean' shutdown: the ccs server
// might terminate before the client, leading to a socket
// error at the client end. however, since this is the
// last thing that happens in the simulation, perhaps
// it is tolerable, but certainly not elegant. 
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

  CcsId ccsId(id.value());
  CkPrintf("CopyOperation::exec doCopy ret %u\n", ccsId.value());
  CcsSendDelayedReply(delayedReply_, sizeof(CcsId), &ccsId);
}

void FindOperation::exec(Shell *shell){
  std::string path;
  CcsPackUnpack<std::string>::unpackHandler(msg_, path);

  ObjId id = shell->doFind(path);
  CcsObjId ccsId(id.id.value(), id.dataId.value());
  SetGet1CcsWrapper< CcsObjId > wrapper(ccsId, !(id == ObjId()), CkMyPe());

  // since reduction requires a special (see SetGetCcsServer) merge function, pack
  unsigned int size;
  char *toSend = CcsPackUnpack< SetGet1CcsWrapper< CcsObjId > >::pack(wrapper, size);
  CcsSendDelayedReply(delayedReply_, size, toSend);

  delete[] toSend;
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

  CcsId ccsId(id.value());
  CcsSendDelayedReply(delayedReply_, sizeof(CcsId), &ccsId);
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

void CinfosOperation::exec(Shell *shell){
  map< string, Cinfo * > &tab = Cinfo::cinfoMap();
  map< string, Cinfo * >::iterator cit;

  // we create an array of ccscinfos instead of a table, so that
  // we can store indices from the base pointer of the array to
  // identify parent ccscinfos.
  vector< CcsCinfo > toPack;
  toPack.resize(tab.size());
  int sindex = 0;

  // stores indices of Cinfos corresponding to 
  // descriptive names
  map< string, int > tmp;

  // populate tmp table
  for(cit = tab.begin(); cit != tab.end(); ++cit){
    // get a cinfo from the table
    Cinfo *cinfo = cit->second;
    // pick a corresponding target ccscinfo in the array
    CcsCinfo *ccsCinfo = &toPack[sindex];
    // fill in the target's fields
    cinfo->fillCcsCinfo(ccsCinfo);
    // record the filled-in ccscinfo in a table for 
    // quick recall when setting parent pointers (indices)
    tmp[cit->first] = sindex++;
  }

  // now set parent ccscinfos
  for(cit = tab.begin(); cit != tab.end(); ++cit){
    // get a cinfo
    Cinfo *cinfo = cit->second;
    // get the corresponding ccscinfo; here, we could have
    // directly indexed into the toPack array as well
    CcsCinfo *ccsCinfo = &toPack[tmp[cit->first]];
    // get the parent ccscinfo based on the name of the parent 
    // of the corresponding cinfo. this is why the tmp table
    // is needed in the first place
    int parentIndex = -1;
    const Cinfo *parentCinfo = cinfo->baseCinfo();
    if(parentCinfo != NULL){
      parentIndex = tmp[parentCinfo->name()];
    }
    // set index of parent ccscinfo relative to base ptr of toPack array
    ccsCinfo->setBaseCinfoIndex(parentIndex);
  }

  // now do the actual serialization
  PUP::sizer psz;
  psz | toPack;

  char *packed = new char[psz.size()];
  PUP::toMem pmem(packed);
  pmem | toPack;


  CcsSendDelayedReply(delayedReply_, psz.size(), packed);

  delete[] packed;
}

