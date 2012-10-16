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


#include "ShellProxy.h"
#include "ShellProxyHelpers.h"
#include "charm++.h"
#include "pup.h"
#include "pup_stl.h"

#include "svn_revision.h"

#include <utility>

const char *ShellProxy::setCweHandlerString = "setCwe"; 
const char *ShellProxy::getCweHandlerString = "getCwe"; 
const char *ShellProxy::doCreateHandlerString = "doCreate"; 
const char *ShellProxy::doDeleteHandlerString = "doDelete"; 
const char *ShellProxy::doAddMsgHandlerString = "doAddMsg"; 
const char *ShellProxy::doQuitHandlerString = "doQuit"; 
const char *ShellProxy::doStartHandlerString = "doStart"; 
const char *ShellProxy::doReinitHandlerString = "doReinit"; 
const char *ShellProxy::doStopHandlerString = "doStop"; 
const char *ShellProxy::doTerminateHandlerString = "doTerminate"; 
const char *ShellProxy::doMoveHandlerString = "doMove"; 
const char *ShellProxy::doCopyHandlerString = "doCopy"; 
const char *ShellProxy::doFindHandlerString = "doFind"; 
const char *ShellProxy::doUseClockHandlerString = "doUseClock"; 
const char *ShellProxy::doLoadModelHandlerString = "doLoadModel"; 
const char *ShellProxy::doWriteSBMLHandlerString = "doWriteSBML"; 
const char *ShellProxy::doSyncDataHandlerString = "doSyncDataHandler"; 
const char *ShellProxy::doReacDiffMeshHandlerString = "doReacDiffMesh"; 
const char *ShellProxy::doSetClockHandlerString = "doSetClock"; 
const char *ShellProxy::doCleanSimulationHandlerString = "doCleanSimulation"; 
const char *ShellProxy::doAdoptHandlerString = "doAdopt"; 



ShellProxy::ShellProxy(){
}

ShellProxy::~ShellProxy(){
}

std::string ShellProxy::doVersion(){
  return MOOSE_VERSION;
}

std::string ShellProxy::doRevision(){
  return SVN_REVISION;
}

void ShellProxy::setCwe(Id cwe){
  // Id's do not have to be packd; can be treated as an already contiguous run of bytes
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::setCweHandlerString, sizeof(Id), &cwe);

  bool r;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &r, MOOSE_CCS_TIMEOUT) < 0);
}

Id ShellProxy::getCwe(){
  const char *dummy = "dummy";
  CcsSendRequest(&shellServer_, ShellProxy::getCweHandlerString, 0, strlen(dummy)+1, dummy);

  Id ret;
  while(CcsRecvResponse(&shellServer_, sizeof(Id), &ret, MOOSE_CCS_TIMEOUT) < 0);
  return ret;
}

bool ShellProxy::isRunning() const {
  return isRunning_;
}

Id ShellProxy::doCreate(string type, Id parent, string name, vector<int> dimensions, bool isGlobal, bool qFlag){
  unsigned int size;
  CreateStruct create(type, parent, name, dimensions, isGlobal, qFlag);
  char *toSend = CcsPackUnpack< CreateStruct >::pack(create, size);

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doCreateHandlerString, size, toSend);
  delete[] toSend;

  Id createdId;
  while(CcsRecvResponse(&shellServer_, sizeof(Id), &createdId, MOOSE_CCS_TIMEOUT) < 0);

  return createdId;
}

bool ShellProxy::doDelete(Id id, bool qFlag){
  std::pair<Id, bool> toSend = make_pair(id, qFlag);
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doDeleteHandlerString, sizeof(toSend), &toSend);

  bool ret;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) < 0);

  return ret;
}

MsgId ShellProxy::doAddMsg(const string &msgType, ObjId src, const string &srcField, ObjId dest, const string &destField, bool qFlag){
  unsigned int size;
  AddMsgStruct addmsg(msgType, src, srcField, dest, destField, qFlag);
  char *toSend = CcsPackUnpack< AddMsgStruct >::pack(addmsg, size);

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doAddMsgHandlerString, size, toSend);
  delete[] toSend;

  MsgId id;
  while(CcsRecvResponse(&shellServer_, sizeof(MsgId), &id, MOOSE_CCS_TIMEOUT) < 0);

  return id;
}

void ShellProxy::doQuit(bool qFlag){
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doQuitHandlerString, sizeof(bool), &qFlag);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

void ShellProxy::doStart(double runtime, bool qFlag){
  std::pair<double, bool> pr(runtime, qFlag);
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doStartHandlerString, sizeof(pr), &pr);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

void ShellProxy::doNonBlockingStart(double runtime, bool qFlag){
  // not supported for the time being
  CkPrintf("ShellProxy::doNonBlockingStart() not supported\n");
}

void ShellProxy::doReinit(bool qFlag){
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doReinitHandlerString, sizeof(bool), &qFlag);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

void ShellProxy::doStop(bool qFlag){
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doStopHandlerString, sizeof(bool), &qFlag);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

void ShellProxy::doTerminate(bool qFlag){
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doTerminateHandlerString, sizeof(bool), &qFlag);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

void ShellProxy::doMove(Id orig, Id newParent, bool qFlag){
  MoveStruct ms(orig, newParent, qFlag);
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doMoveHandlerString, sizeof(MoveStruct), &ms);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

void ShellProxy::doCopy(Id orig, Id newParent, string newName, unsigned int n, bool toGlobal, bool copyExtMsgs, bool qFlag){
  unsigned int size;
  CopyStruct copystruct(orig, newParent, newName, n, toGlobal, copyExtMsgs, qFlag);
  char *toSend = CcsPackUnpack< CopyStruct >::pack(copystruct, size);

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doCopyHandlerString, size, toSend);
  delete[] toSend;

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

ObjId ShellProxy::doFind(string &path){
  unsigned int size;
  char *toSend = CcsPackUnpack< string >::pack(path, size);

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doFindHandlerString, size, toSend);
  delete[] toSend;

  ObjId ret;
  while(CcsRecvResponse(&shellServer_, sizeof(ObjId), &ret, MOOSE_CCS_TIMEOUT) < 0);
  return ret;
}

void ShellProxy::doUseClock(string path, string field, unsigned int tick, bool qFlag){
  unsigned int size;
  UseClockStruct use(path, field, tick, qFlag);
  char *toSend = CcsPackUnpack< UseClockStruct >::pack(use, size);

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doUseClockHandlerString, size, toSend);
  delete[] toSend;

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

Id ShellProxy::doLoadModel(const string& fname, const string& modelpath, const string& solverClass, bool qFlag){
  unsigned int size;
  LoadModelStruct load(fname, modelpath, solverClass, qFlag);
  char *toSend = CcsPackUnpack< LoadModelStruct >::pack(load, size);

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doLoadModelHandlerString, size, toSend);
  delete[] toSend;

  Id id;
  while(CcsRecvResponse(&shellServer_, sizeof(Id), &id, MOOSE_CCS_TIMEOUT) < 0);
  return id;
}

int ShellProxy::doWriteSBML(const string &fname, const string &modelpath, bool qFlag){
  unsigned int size;
  WriteSbmlStruct write(fname, modelpath, qFlag);
  char *toSend = CcsPackUnpack< WriteSbmlStruct >::pack(write, size); 

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doWriteSBMLHandlerString, size, toSend);
  delete[] toSend;

  int ret;
  while(CcsRecvResponse(&shellServer_, sizeof(int), &ret, MOOSE_CCS_TIMEOUT) < 0);
  return ret;
}

void ShellProxy::doSyncDataHandler(Id tgt){
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doSyncDataHandlerString, sizeof(Id), &tgt);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

void ShellProxy::doReacDiffMesh(Id baseCompartment){
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doReacDiffMeshHandlerString, sizeof(Id), &baseCompartment);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

void ShellProxy::clearRestructuringQ(){
  // XXX - what does this do?
  std::cerr << "ShellProxy::clearRestructuringQ() not implemented" << std::endl;
}

void ShellProxy::doSetClock(unsigned int tickNum, double dt, bool qFlag){
  SetClockStruct setClock(tickNum, dt, qFlag);
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doSetClockHandlerString, sizeof(SetClockStruct), &setClock);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

void ShellProxy::cleanSimulation(){
  const char *dummy = "dummy";
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doCleanSimulationHandlerString, strlen(dummy)+1, dummy);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

bool ShellProxy::adopt(Id parent, Id child){
  AdoptStruct adoptStruct(parent, child);
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doAdoptHandlerString, sizeof(AdoptStruct), &adoptStruct);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);

  return b;
}


