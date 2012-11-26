#ifndef SET_GET_CCS_SERVER_H
#define SET_GET_CCS_SERVER_H

#include <string>
using namespace std;

#include "../basecode/CcsPackUnpack.h"
#include "SetGetWrapper.h"
#include "SetGet.h"
#include "SetGetCcsClient.h"
#include "pup_stl.h"

#define SETGET_CCS_VERBOSE CkPrintf

class SetGetCcsServer {
  public:
    static void strGet_handler(char *msg){
      SetGetCcsClient::Args args;
      CcsPackUnpack< SetGetCcsClient::Args >::unpackHandler(msg, args);
      CmiFree(msg);

      string ret;
      bool success = SetGet::strGet(ObjId(args.dest_), args.field_, ret);
      SETGET_CCS_VERBOSE("[%d] SetGetCcsServer::strGet success:%d\n", CkMyPe(), success);
      unsigned int size;
      SetGet1CcsWrapper< string > wrapper(ret, success);
      msg = CcsPackUnpack< SetGet1CcsWrapper< string > >::pack(wrapper, size); 
      CcsSendReply(size, msg);
      delete[] msg;
    }


    static void strSet_handler(char *msg){
      SetGet1CcsClient< string >::Args args;
      CcsPackUnpack< SetGet1CcsClient< string >::Args >::unpackHandler(msg, args);
      CmiFree(msg);

      bool ret = SetGet::strSet(ObjId(args.dest_), args.field_, args.values_[0]); 
      SETGET_CCS_VERBOSE("[%d] SetGetCcsServer::strSet success: %d\n", CkMyPe(), ret);
      CcsSendReply(sizeof(bool), &ret);
    }

// CCS REDUCTION MERGER FUNCTION for packed data types

  template<typename WRAPPER_TYPE>
  static void *merge(int *localSize, void *localContribution, void **remoteContributions, int nRemoteContributions){
    //CkPrintf("[%d] reducer\n", CmiMyPe());
    CcsImplHeader *header = (CcsImplHeader *) (((char *) localContribution) + CmiReservedHeaderSize);

    int msgSize = CmiReservedHeaderSize + sizeof(CcsImplHeader) + ChMessageInt(header->len);
    CmiAssert(*localSize == msgSize);

    // this is the locally contributed data
    PUP::fromMem pmeml(header + 1);
    WRAPPER_TYPE localData;
    pmeml | localData;

    //cout << "reducer localData: " << localData << endl;

    // go through recvd remote contributions and accumulate
    for(int i = 0; i < nRemoteContributions; i++){
      // check whether remote contributions are sane     
      header = (CcsImplHeader *)(((char *) remoteContributions[i]) + CmiReservedHeaderSize);
      PUP::fromMem pmemr(header + 1);
      WRAPPER_TYPE remoteData;
      pmemr | remoteData;

      //cout << "reducer remoteData: " << remoteData << endl;

      // accumulate remote contribution 
      localData += remoteData;
      //CmiFree(remoteContributions[i]);
    }

    PUP::sizer psz;
    psz | localData;

    msgSize = psz.size() + sizeof(CcsImplHeader) + CmiReservedHeaderSize;
    char *replyMsg = (char *) CmiAlloc(msgSize);

    // copy converse header from local contribution
    //memcpy(replyMsg, localContribution, CmiReservedHeaderSize);

    // copy ccs header from local contribution
    memcpy(replyMsg + CmiReservedHeaderSize, 
        ((char *) localContribution) + CmiReservedHeaderSize, 
        sizeof(CcsImplHeader));
    header = (CcsImplHeader *) (replyMsg + CmiReservedHeaderSize);
    // set len field of CCS header
    header->len = ChMessageInt_new(psz.size());

    // copy accumulated data into allocated message
    PUP::toMem pmem(header + 1);
    pmem | localData;

    //cout << "reducer size: " << psz.size() <<  " result: " << localData << endl;


    /*
       {
    // debug
    PUP::fromMem pmemDebug(header + 1);
    WRAPPER_TYPE dataDebug;
    pmemDebug | dataDebug;

    cout << "reducer debug: " << dataDebug << endl;
    }
     */

    //CmiFree(localContribution);
    return replyMsg;
  }
};

class SetGet0CcsServer {
  public:
  static void set_handler(char *msg){
    SetGetCcsClient::Args args;
    CcsPackUnpack<SetGetCcsClient::Args>::unpackHandler(msg, args);
    CmiFree(msg);

    bool ret = SetGet0::set(ObjId(args.dest_), args.field_);
    SETGET_CCS_VERBOSE("[%d] SetGet0CcsServer::set success: %d\n", CkMyPe(), ret);
    CcsSendReply(sizeof(bool), &ret);
  }
};

template<typename A>
class SetGet1CcsServer : public SetGetCcsServer {
  public:
  static void set_handler(char *msg){
    typename SetGet1CcsClient< A >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< A >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    bool ret = SetGet1<A>::set(ObjId(args.dest_), args.field_, args.values_[0]);
    SETGET_CCS_VERBOSE("[%d] SetGet1CcsServer<A>::set success: %d\n", CkMyPe(), ret);
    CcsSendReply(sizeof(bool), &ret);
  }

  static void setVec_handler(char *msg){
    typename SetGet1CcsClient< A >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< A >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    bool ret = SetGet1<A>::setVec(Id(args.dest_.id), args.field_, args.values_);
    SETGET_CCS_VERBOSE("[%d] SetGet1CcsServer<A>::setVec success: %d\n", CkMyPe(), ret);
    CcsSendReply(sizeof(bool), &ret);
  }


};

template<>
void SetGet1CcsServer<CcsId>::set_handler(char *msg){
  SetGet1CcsClient< CcsId >::Args args;
  CcsPackUnpack< SetGet1CcsClient< CcsId >::Args >::unpackHandler(msg, args);
  CmiFree(msg);

  bool ret = SetGet1<Id>::set(ObjId(args.dest_), args.field_, Id(args.values_[0]));
  SETGET_CCS_VERBOSE("[%d] SetGet1CcsServer<CcsId>::setVec success: %d\n", CkMyPe(), ret);
  CcsSendReply(sizeof(bool), &ret);
}

template<>
void SetGet1CcsServer<CcsObjId>::set_handler(char *msg){
  SetGet1CcsClient< CcsObjId >::Args args;
  CcsPackUnpack< SetGet1CcsClient< CcsObjId >::Args >::unpackHandler(msg, args);
  CmiFree(msg);

  bool ret = SetGet1<ObjId>::set(ObjId(args.dest_), args.field_, ObjId(args.values_[0]));
  SETGET_CCS_VERBOSE("[%d] SetGet1CcsServer<CcsObjId>::set success: %d\n", CkMyPe(), ret);
  CcsSendReply(sizeof(bool), &ret);
}



template<typename A>
class FieldCcsServer : public SetGet1CcsServer<A> {
  public:
  static void get_handler(char *msg){
    SetGetCcsClient::Args args;
    CcsPackUnpack<SetGetCcsClient::Args>::unpackHandler(msg, args);
    CmiFree(msg);

    A ret;
    bool success = false;

    ObjId oid(args.dest_);
    // FIXME - must differentiate between get(ccsid)
    // and get(ccsobjid): test should only be performed for ccsobjid
    // FIXME - for which other set/get calls must we perform this check?
    // since this ccs call is going to be broadcast, the appropriate
    // node on which the object is present should be the only one
    // trying to invoke get() on it. 
    if(oid.isDataHere()){
      success = Field<A>::get(ObjId(args.dest_), args.field_, ret); 
    }
    
    SETGET_CCS_VERBOSE("[%d] FieldCcsServer<A>::get success: %d\n", CkMyPe(), success);
    // A might have members that need to be wrapper
    unsigned int size;
    SetGet1CcsWrapper< A > wrapper(ret, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< A > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }

  static void getVec_handler(char *msg){
    SetGetCcsClient::Args args; 
    CcsPackUnpack<SetGetCcsClient::Args>::unpackHandler(msg, args);
    CmiFree(msg);

    vector< A > ret;
    bool success = Field<A>::getVec(Id(args.dest_.id), args.field_, ret);
    SETGET_CCS_VERBOSE("[%d] FieldCcsServer<A>::getVec success: %d\n", CkMyPe(), success);

    // A might have members that need to be wrapper
    unsigned int size;
    SetGet1CcsWrapper< vector< A > > wrapper(ret, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< vector< A > > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

// specializations for returning CcsId/CcsObjId. These are needed because
// we must convert from Id/ObjId to CcsId/CcsObjId before replying
template<>
void FieldCcsServer<CcsId>::get_handler(char *msg){
  SetGetCcsClient::Args args;
  CcsPackUnpack<SetGetCcsClient::Args>::unpackHandler(msg, args);
  CmiFree(msg);

  Id ret;
  bool success = Field<Id>::get(ObjId(args.dest_), args.field_, ret);
  SETGET_CCS_VERBOSE("[%d] FieldCcsServer<CcsId>::get success: %d\n", CkMyPe(), success);

  CcsId reply;
  reply.id_ = ret.value();

  unsigned int size;
  SetGet1CcsWrapper<CcsId> wrapper(reply, success);
  msg = CcsPackUnpack< SetGet1CcsWrapper< CcsId > >::pack(wrapper, size);

  CcsSendReply(size, msg);
  delete[] msg;
}

template<>
void FieldCcsServer<vector<CcsId> >::get_handler(char *msg){
  SetGetCcsClient::Args args;
  CcsPackUnpack<SetGetCcsClient::Args>::unpackHandler(msg, args);
  CmiFree(msg);

  vector<Id> ret;
  bool success = Field<vector<Id> >::get(ObjId(args.dest_), args.field_, ret);
  SETGET_CCS_VERBOSE("[%d] FieldCcsServer<vector<CcsId>>::get success: %d\n", CkMyPe(), success);

  vector<CcsId> reply;
  for(int i = 0; i < ret.size(); i++){
    reply.push_back(CcsId(ret[i].value()));
  }

  unsigned int size;
  SetGet1CcsWrapper<vector<CcsId> > wrapper(reply, success);
  msg = CcsPackUnpack< SetGet1CcsWrapper< vector<CcsId> > >::pack(wrapper, size);

  CcsSendReply(size, msg);
  delete[] msg;
}

template<>
void FieldCcsServer<CcsId>::getVec_handler(char *msg){
  SetGetCcsClient::Args args; 
  CcsPackUnpack<SetGetCcsClient::Args>::unpackHandler(msg, args);
  CmiFree(msg);

  vector< Id > ret;
  bool success = Field<Id>::getVec(Id(args.dest_.id), args.field_, ret);
  SETGET_CCS_VERBOSE("[%d] FieldCcsServer<CcsId>::getVec success: %d\n", CkMyPe(), success);

  vector< CcsId > reply;
  for(int i = 0; i < ret.size(); i++){
    reply.push_back(CcsId(ret[i].value()));
  }

  // A might have members that need to be wrapper
  unsigned int size;
  SetGet1CcsWrapper< vector< CcsId > > wrapper(reply, success);
  msg = CcsPackUnpack< SetGet1CcsWrapper< vector< CcsId > > >::pack(wrapper, size);

  CcsSendReply(size, msg);
  delete[] msg;
}


template<>
void FieldCcsServer<CcsObjId>::get_handler(char *msg){
  SetGetCcsClient::Args args;
  CcsPackUnpack<SetGetCcsClient::Args>::unpackHandler(msg, args);
  CmiFree(msg);

  ObjId ret;
  bool success = Field<ObjId>::get(ObjId(args.dest_), args.field_, ret);
  SETGET_CCS_VERBOSE("[%d] FieldCcsServer<CcsObjId>::getVec success: %d\n", CkMyPe(), success);

  CcsObjId reply;
  reply.id = CcsId(ret.id.value());
  reply.dataId = CcsDataId(ret.dataId.value());

  unsigned int size;
  SetGet1CcsWrapper<CcsObjId> wrapper(reply, success);
  msg = CcsPackUnpack< SetGet1CcsWrapper< CcsObjId > >::pack(wrapper, size);

  CcsSendReply(size, msg);
  delete[] msg;
}

template<>
void FieldCcsServer<vector<CcsObjId> >::get_handler(char *msg){
  SetGetCcsClient::Args args;
  CcsPackUnpack<SetGetCcsClient::Args>::unpackHandler(msg, args);
  CmiFree(msg);

  vector<ObjId> ret;
  bool success = Field<vector<ObjId> >::get(ObjId(args.dest_), args.field_, ret);
  SETGET_CCS_VERBOSE("[%d] FieldCcsServer<vector<CcsObjId>>::get success: %d\n", CkMyPe(), success);

  vector<CcsObjId> reply;
  for(int i = 0; i < ret.size(); i++){
    reply.push_back(CcsObjId(ret[i].id.value(), ret[i].dataId.value()));
  }

  unsigned int size;
  SetGet1CcsWrapper<vector<CcsObjId> > wrapper(reply, success);
  msg = CcsPackUnpack< SetGet1CcsWrapper< vector<CcsObjId> > >::pack(wrapper, size);

  CcsSendReply(size, msg);
  delete[] msg;
}

template<>
void FieldCcsServer<CcsObjId>::getVec_handler(char *msg){
  SetGetCcsClient::Args args; 
  CcsPackUnpack<SetGetCcsClient::Args>::unpackHandler(msg, args);
  CmiFree(msg);

  vector< ObjId > ret;
  bool success = Field<ObjId>::getVec(Id(args.dest_.id), args.field_, ret);
  SETGET_CCS_VERBOSE("[%d] FieldCcsServer<CcsObjId>::getVec success: %d\n", CkMyPe(), success);

  vector< CcsObjId > reply;
  for(int i = 0; i < ret.size(); i++){
    reply.push_back(CcsObjId(ret[i].id.value(), ret[i].dataId.value()));
  }

  // A might have members that need to be wrapper
  unsigned int size;
  SetGet1CcsWrapper< vector< CcsObjId > > wrapper(reply, success);
  msg = CcsPackUnpack< SetGet1CcsWrapper< vector< CcsObjId > > >::pack(wrapper, size);

  CcsSendReply(size, msg);
  delete[] msg;
}

template<typename A1, typename A2> 
class SetGet2CcsServer : public SetGetCcsServer {
  public:
  static void set_handler(char *msg){
    typename SetGet2CcsClient<A1, A2>::Args args;
    CcsPackUnpack< typename SetGet2CcsClient<A1, A2>::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    bool ret = SetGet2<A1, A2>::set(ObjId(args.dest_), args.field_, args.a1_[0], args.a2_[0]);
    SETGET_CCS_VERBOSE("[%d] SetGet2CcsServer<A1,A2>::set success: %d\n", CkMyPe(), ret);
    CcsSendReply(sizeof(bool), &ret);
  }

  static void setVec_handler(char *msg){
    typename SetGet2CcsClient<A1, A2>::Args args;
    CcsPackUnpack< typename SetGet2CcsClient<A1, A2>::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    bool ret = SetGet2<A1, A2>::setVec(ObjId(args.dest_), args.field_, args.a1_, args.a2_);
    SETGET_CCS_VERBOSE("[%d] SetGet2CcsServer<A1,A2>::setVec success: %d\n", CkMyPe(), ret);
    CcsSendReply(sizeof(bool), &ret);
  }


};

template<typename L, typename A> 
class LookupFieldCcsServer : public SetGet2CcsServer<L, A> {
  public:
  static void get_handler(char *msg){
    typename SetGet1CcsClient< L >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< L >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    A ret;
    bool success = LookupField<L, A>::get(ObjId(args.dest_), args.field_, args.values_[0], ret); 
    SETGET_CCS_VERBOSE("[%d] LookupFieldCcsServer<L,A>::get success: %d\n", CkMyPe(), success);
    // A might have members that need to be packed
    unsigned int size;
    SetGet1CcsWrapper< A > wrapper(ret, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< A > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }

  static void getVec_handler(char *msg){
    SETGET_CCS_VERBOSE("[%d] LookupFieldCcsServer<L,A>::getVec\n", CkMyPe());
    typename SetGet1CcsClient< L >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< L >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    vector< A > ret;
    LookupField<L, A>::getVec(Id(args.dest_.id), args.field_, args.values_, ret); 
    // A might have members that need to be packed
    unsigned int size;
    SetGet1CcsWrapper< vector< A > > wrapper(ret, true);
    msg = CcsPackUnpack< SetGet1CcsWrapper< vector< A > > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

template<typename L>
class LookupFieldCcsServer<L, CcsId> : public SetGet2CcsServer<L, CcsId> {
  public:
  static void get_handler(char *msg){
    typename SetGet1CcsClient< L >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< L >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    Id ret;
    bool success = LookupField<L, Id>::get(ObjId(args.dest_), args.field_, args.values_[0], ret); 
    SETGET_CCS_VERBOSE("[%d] LookupFieldCcsServer<L,CcsId>::get success: %d\n", CkMyPe(), success);

    CcsId reply(ret.value());
    unsigned int size;
    SetGet1CcsWrapper< CcsId > wrapper(reply, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< CcsId > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

template<typename A>
class LookupFieldCcsServer<CcsId, A> : public SetGet2CcsServer<CcsId, A> {
  public:
  static void get_handler(char *msg){
    SetGet1CcsClient< CcsId >::Args args;
    CcsPackUnpack< SetGet1CcsClient< CcsId >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    A ret;
    bool success = LookupField<Id, A>::get(ObjId(args.dest_), args.field_, Id(args.values_[0]), ret); 
    SETGET_CCS_VERBOSE("[%d] LookupFieldCcsServer<CcsId,A>::get success: %d\n", CkMyPe(), success);
    // A might have members that need to be packed

    unsigned int size;
    SetGet1CcsWrapper< A > wrapper(ret, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< A > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

template<>
class LookupFieldCcsServer<CcsId, CcsId> : public SetGet2CcsServer<CcsId, CcsId> {
  public:
  static void get_handler(char *msg){
    SetGet1CcsClient< CcsId >::Args args;
    CcsPackUnpack< SetGet1CcsClient< CcsId >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    Id ret;
    bool success = LookupField<Id, Id>::get(ObjId(args.dest_), args.field_, Id(args.values_[0]), ret); 
    SETGET_CCS_VERBOSE("[%d] LookupFieldCcsServer<CcsId,CcsId>::get success: %d\n", CkMyPe(), success);
    // A might have members that need to be packed

    CcsId reply(ret.value());
    unsigned int size;
    SetGet1CcsWrapper< CcsId > wrapper(reply, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< CcsId > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

template<>
class LookupFieldCcsServer<CcsId, CcsObjId> : public SetGet2CcsServer<CcsId, CcsObjId> {
  public:
  static void get_handler(char *msg){
    SetGet1CcsClient< CcsId >::Args args;
    CcsPackUnpack< SetGet1CcsClient< CcsId >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    ObjId ret;
    bool success = LookupField<Id, ObjId>::get(ObjId(args.dest_), args.field_, Id(args.values_[0]), ret); 
    SETGET_CCS_VERBOSE("[%d] LookupFieldCcsServer<CcsId,CcsObjId>::get success: %d\n", CkMyPe(), success);
    // A might have members that need to be packed

    CcsObjId reply(ret.id.value(), ret.dataId.value());
    unsigned int size;
    SetGet1CcsWrapper< CcsObjId > wrapper(reply, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< CcsObjId > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

template<>
class LookupFieldCcsServer<CcsObjId, CcsId> : public SetGet2CcsServer<CcsObjId, CcsId> {
  public:
  static void get_handler(char *msg){
    SetGet1CcsClient< CcsObjId >::Args args;
    CcsPackUnpack< SetGet1CcsClient< CcsObjId >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    Id ret;
    bool success = LookupField<ObjId, Id>::get(ObjId(args.dest_), args.field_, ObjId(args.values_[0]), ret); 
    SETGET_CCS_VERBOSE("[%d] LookupFieldCcsServer<CcsObjId,CcsId>::get success: %d\n", CkMyPe(), success);
    // A might have members that need to be packed

    CcsId reply(ret.value());
    unsigned int size;
    SetGet1CcsWrapper< CcsId > wrapper(reply, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< CcsId > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

template<typename L>
class LookupFieldCcsServer<L, CcsObjId> : public SetGet2CcsServer<L, CcsObjId> {
  public:
  static void get_handler(char *msg){
    typename SetGet1CcsClient< L >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< L >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    ObjId ret;
    bool success = LookupField<L, ObjId>::get(ObjId(args.dest_), args.field_, args.values_[0], ret); 
    SETGET_CCS_VERBOSE("[%d] LookupFieldCcsServer<L,CcsObjId>::get success: %d\n", CkMyPe(), success);
    // A might have members that need to be packed

    CcsObjId reply(ret.id.value(), ret.dataId.value());
    unsigned int size;
    SetGet1CcsWrapper< CcsObjId > wrapper(reply, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< CcsObjId > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

template<typename A>
class LookupFieldCcsServer<CcsObjId, A> : public SetGet2CcsServer<CcsObjId, A> {
  public:
  static void get_handler(char *msg){
    SetGet1CcsClient< CcsObjId >::Args args;
    CcsPackUnpack< SetGet1CcsClient< CcsObjId >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    A ret;
    bool success = LookupField<ObjId, A>::get(ObjId(args.dest_), args.field_, ObjId(args.values_[0]), ret); 
    SETGET_CCS_VERBOSE("[%d] LookupFieldCcsServer<CcsObjId,A>::get success: %d\n", CkMyPe(), success);
    // A might have members that need to be packed

    unsigned int size;
    SetGet1CcsWrapper< A > wrapper(ret, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< A > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

template<>
class LookupFieldCcsServer<CcsObjId, CcsObjId> : public SetGet2CcsServer<CcsObjId, CcsObjId> {
  public:
  static void get_handler(char *msg){
    SetGet1CcsClient< CcsObjId >::Args args;
    CcsPackUnpack< SetGet1CcsClient< CcsObjId >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    ObjId ret;
    bool success = LookupField<ObjId, ObjId>::get(ObjId(args.dest_), args.field_, ObjId(args.values_[0]), ret); 
    SETGET_CCS_VERBOSE("[%d] LookupFieldCcsServer<CcsObjId,CcsObjId>::get success: %d\n", CkMyPe(), success);
    // A might have members that need to be packed

    CcsObjId reply(ret.id.value(), ret.dataId.value());
    unsigned int size;
    SetGet1CcsWrapper< CcsObjId > wrapper(reply, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< CcsObjId > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

template<typename A1, typename A2, typename A3> 
class SetGet3CcsServer : public SetGetCcsServer {
  public:
  static void set_handler(char *msg){
    typename SetGet3CcsClient< A1, A2, A3 >::Args args;
    CcsPackUnpack< typename SetGet3CcsClient< A1, A2, A3 >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    bool ret = SetGet3<A1, A2, A3>::set(ObjId(args.dest_), args.field_, args.a1_, args.a2_, args.a3_);
    SETGET_CCS_VERBOSE("[%d] SetGet3CcsServer<A1,A2,A3>::set success: %d\n", CkMyPe(), ret);
    CcsSendReply(sizeof(bool), &ret);
  }
};


template<typename A1, typename A2, typename A3, typename A4> 
class SetGet4CcsServer : public SetGetCcsServer {
  static void set_handler(char *msg){
    typename SetGet4CcsClient< A1, A2, A3, A4 >::Args args;
    CcsPackUnpack< typename SetGet4CcsClient< A1, A2, A3, A4 >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    bool ret = SetGet4<A1, A2, A3, A4>::set(ObjId(args.dest_), args.field_, args.a1_, args.a2_, args.a3_, args.a4_);
    SETGET_CCS_VERBOSE("[%d] SetGet4CcsServer<A1,A2,A3,A4>::set success: %d\n", CkMyPe(), ret);
    CcsSendReply(sizeof(bool), &ret);
  }
};


template< class A1, class A2, class A3, class A4, class A5 > class SetGet5CcsServer : public SetGetCcsServer {
  static void set_handler(char *msg){
    typename SetGet5CcsClient< A1, A2, A3, A4, A5 >::Args args;
    CcsPackUnpack< typename SetGet5CcsClient< A1, A2, A3, A4, A5 >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    bool ret = SetGet5<A1, A2, A3, A4, A5>::set(ObjId(args.dest_), args.field_, args.a1_, args.a2_, args.a3_, args.a4_, args.a5_);
    SETGET_CCS_VERBOSE("[%d] SetGet5CcsServer<A1,A2,A3,A4,A5>::set success: %d\n", CkMyPe(), ret);
    CcsSendReply(sizeof(bool), &ret);
  }
};


template< class A1, class A2, class A3, class A4, class A5, class A6 > class SetGet6CcsServer : public SetGetCcsServer {
  static void set_handler(char *msg){
    typename SetGet6CcsClient< A1, A2, A3, A4, A5, A6 >::Args args;
    CcsPackUnpack< typename::SetGet6CcsClient< A1, A2, A3, A4, A5, A6 >::Args >::unpackHandler(msg, args);
    CmiFree(msg);

    bool ret = SetGet6<A1, A2, A3, A4, A5, A6>::set(ObjId(args.dest_), args.field_, args.a1_, args.a2_, args.a3_, args.a4_, args.a5_, args.a6_);
    SETGET_CCS_VERBOSE("[%d] SetGet6CcsServer<A1,A2,A3,A4,A5,A6>::set success: %d\n", CkMyPe(), ret);
    CcsSendReply(sizeof(bool), &ret);
  }
};

#endif // SET_GET_CCS_SERVER_H
