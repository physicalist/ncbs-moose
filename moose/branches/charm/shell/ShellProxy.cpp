// need these includes for Shell
#if 0
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
#endif

#include "CcsId.h"
#include "CcsObjId.h"
#include "CcsDataId.h"

#include "../basecode/CcsPackUnpack.h"
#include "ShellProxy.h"
#include "ShellProxyHelpers.h"
#include "converse.h"
#include "pup.h"
#include "pup_stl.h"

#include "../basecode/svn_revision.h"

#include <utility>
#include <iostream>

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

const char *ShellProxy::doGetPathHandlerString = "doGetPath"; 
const char *ShellProxy::doGetObjIdPathHandlerString = "doGetObjIdPath"; 
const char *ShellProxy::doGetIsValidHandlerString = "doGetIsValid"; 
const char *ShellProxy::doWildcardHandlerString = "doWildcard"; 
const char *ShellProxy::doGetMsgMgrHandlerString = "doGetMsgMgr"; 



void ShellProxy::ccsInit(string serverName, int serverPort){
  CcsConnect(&shellServer_, serverName.c_str(), serverPort, NULL);
}

std::string ShellProxy::doVersion(){
  return MOOSE_VERSION;
}

std::string ShellProxy::doRevision(){
  return SVN_REVISION;
}

void ShellProxy::setCwe(CcsId cwe){
  // Id's do not have to be packd; can be treated as an already contiguous run of bytes
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::setCweHandlerString, sizeof(CcsId), &cwe);

  bool r;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &r, MOOSE_CCS_TIMEOUT) < 0);
}

CcsId ShellProxy::getCwe(){
  const char *dummy = "dummy";
  CcsSendRequest(&shellServer_, ShellProxy::getCweHandlerString, 0, strlen(dummy)+1, dummy);

  CcsId ret;
  while(CcsRecvResponse(&shellServer_, sizeof(CcsId), &ret, MOOSE_CCS_TIMEOUT) < 0);
  return ret;
}

bool ShellProxy::isRunning() const {
  return isRunning_;
}

CcsId ShellProxy::doCreate(string type, CcsId parent, string name, vector<int> dimensions, bool isGlobal, bool qFlag){
  unsigned int size;
  CreateStruct create(type, parent, name, dimensions, isGlobal, qFlag);
  char *toSend = CcsPackUnpack< CreateStruct >::pack(create, size);

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doCreateHandlerString, size, toSend);
  delete[] toSend;

  CcsId createdId;
  while(CcsRecvResponse(&shellServer_, sizeof(CcsId), &createdId, MOOSE_CCS_TIMEOUT) < 0);

  return createdId;
}

bool ShellProxy::doDelete(CcsId id, bool qFlag){
  std::pair<CcsId, bool> toSend = make_pair(id, qFlag);
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doDeleteHandlerString, sizeof(toSend), &toSend);

  bool ret;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) < 0);

  return ret;
}

MsgId ShellProxy::doAddMsg(const string &msgType, CcsObjId src, const string &srcField, CcsObjId dest, const string &destField, bool qFlag){
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
  std::cout << "ShellProxy::doNonBlockingStart() not supported" << endl;
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

void ShellProxy::doMove(CcsId orig, CcsId newParent, bool qFlag){
  MoveStruct ms(orig, newParent, qFlag);
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doMoveHandlerString, sizeof(MoveStruct), &ms);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

CcsId ShellProxy::doCopy(CcsId orig, CcsId newParent, string newName, unsigned int n, bool toGlobal, bool copyExtMsgs, bool qFlag){
  unsigned int size;
  CopyStruct copystruct(orig, newParent, newName, n, toGlobal, copyExtMsgs, qFlag);
  char *toSend = CcsPackUnpack< CopyStruct >::pack(copystruct, size);

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doCopyHandlerString, size, toSend);
  delete[] toSend;

  CcsId id;
  while(CcsRecvResponse(&shellServer_, sizeof(CcsId), &id, MOOSE_CCS_TIMEOUT) < 0);

  return id;
}

CcsObjId ShellProxy::doFind(string &path){
  unsigned int size;
  char *toSend = CcsPackUnpack< string >::pack(path, size);

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doFindHandlerString, size, toSend);
  delete[] toSend;

  CcsObjId ret;
  while(CcsRecvResponse(&shellServer_, sizeof(CcsObjId), &ret, MOOSE_CCS_TIMEOUT) < 0);
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

CcsId ShellProxy::doLoadModel(const string& fname, const string& modelpath, const string& solverClass, bool qFlag){
  unsigned int size;
  LoadModelStruct load(fname, modelpath, solverClass, qFlag);
  char *toSend = CcsPackUnpack< LoadModelStruct >::pack(load, size);

  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doLoadModelHandlerString, size, toSend);
  delete[] toSend;

  CcsId id;
  while(CcsRecvResponse(&shellServer_, sizeof(CcsId), &id, MOOSE_CCS_TIMEOUT) < 0);
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

void ShellProxy::doSyncDataHandler(CcsId tgt){
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doSyncDataHandlerString, sizeof(CcsId), &tgt);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
}

void ShellProxy::doReacDiffMesh(CcsId baseCompartment){
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doReacDiffMeshHandlerString, sizeof(CcsId), &baseCompartment);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);
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

bool ShellProxy::adopt(CcsId parent, CcsId child){
  AdoptStruct adoptStruct(parent, child);
  CcsSendBroadcastRequest(&shellServer_, ShellProxy::doAdoptHandlerString, sizeof(AdoptStruct), &adoptStruct);

  bool b;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &b, MOOSE_CCS_TIMEOUT) < 0);

  return b;
}

string ShellProxy::doGetObjIdPath(const CcsObjId &id){
  CcsSendRequest(&shellServer_, ShellProxy::doGetObjIdPathHandlerString, 0, sizeof(CcsObjId), &id);

  string ret;
  int msgSize;
  char *msg;
  while(CcsRecvResponseMsg(&shellServer_, &msgSize, (void **) &msg, MOOSE_CCS_TIMEOUT) < 0);

  CcsPackUnpack<string>::unpack(msg, ret);

  free(msg);

  return ret;
}

string ShellProxy::doGetPath(const CcsId &id){
  CcsSendRequest(&shellServer_, ShellProxy::doGetPathHandlerString, 0, sizeof(CcsId), &id);

  string ret;
  int msgSize;
  char *msg;
  while(CcsRecvResponseMsg(&shellServer_, &msgSize, (void **) &msg, MOOSE_CCS_TIMEOUT) < 0);

  CcsPackUnpack<string>::unpack(msg, ret);

  free(msg);

  return ret;
}

bool ShellProxy::doGetIsValid(const CcsId &id){
  CcsSendRequest(&shellServer_, ShellProxy::doGetIsValidHandlerString, 0, sizeof(CcsId), &id);

  bool ret;
  while(CcsRecvResponse(&shellServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) < 0);

  return ret;
}

void ShellProxy::wildcard(string path, vector<CcsId> &list){
  unsigned int size;
  char *msg = CcsPackUnpack<string>::pack(path, size);
  CcsSendRequest(&shellServer_, ShellProxy::doWildcardHandlerString, 0, size, msg);

  int msgSize;
  while(CcsRecvResponseMsg(&shellServer_, &msgSize, (void **) &msg, MOOSE_CCS_TIMEOUT) < 0);

  CcsPackUnpack<vector<CcsId> >::unpack(msg, list);
  free(msg);
}

CcsObjId ShellProxy::doGetMsgMgr(MsgId mid){
  CcsSendRequest(&shellServer_, ShellProxy::doGetMsgMgrHandlerString, 0, sizeof(MsgId), &mid);

  CcsObjId oid;
  while(CcsRecvResponse(&shellServer_, sizeof(CcsObjId), &oid, MOOSE_CCS_TIMEOUT) < 0);
  return oid;
}

