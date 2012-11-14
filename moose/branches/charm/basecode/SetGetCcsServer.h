#ifndef SET_GET_CCS_SERVER_H
#define SET_GET_CCS_SERVER_H

#include <string>
using namespace std;

#include "../basecode/CcsPackUnpack.h"
#include "SetGetWrapper.h"
#include "SetGet.h"
#include "SetGetCcsClient.h"
#include "pup_stl.h"

// FIXME - start here; 
// create specializations

class SetGetCcsServer {
  public:
    static void strGet_handler(char *msg){
      SetGetCcsClient::Args args;
      CcsPackUnpack< SetGetCcsClient::Args >::unpack(msg, args);
      CmiFree(msg);

      string ret;
      bool success = SetGet::strGet(ObjId(args.dest_), args.field_, ret);
      unsigned int size;
      SetGet1CcsWrapper< string > wrapper(ret, success);
      msg = CcsPackUnpack< SetGet1CcsWrapper< string > >::pack(wrapper, size); 
      CcsSendReply(size, msg);
      delete[] msg;
    }


    static void strSet_handler(char *msg){
      SetGet1CcsClient< string >::Args args;
      CcsPackUnpack< SetGet1CcsClient< string >::Args >::unpack(msg, args);
      CmiFree(msg);

      bool ret = SetGet::strSet(ObjId(args.dest_), args.field_, args.values_[0]); 
      CcsSendReply(sizeof(bool), &ret);
    }
};

class SetGet0CcsServer {
  public:
  static void set_handler(char *msg){
    SetGetCcsClient::Args args;
    CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
    CmiFree(msg);

    bool ret = SetGet0::set(ObjId(args.dest_), args.field_);
    CcsSendReply(sizeof(bool), &ret);
  }
};

template<typename A>
class SetGet1CcsServer : public SetGetCcsServer {
  public:
  static void set_handler(char *msg){
    typename SetGet1CcsClient< A >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< A >::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = SetGet1<A>::set(ObjId(args.dest_), args.field_, args.values_[0]);
    CcsSendReply(sizeof(bool), &ret);
  }

  static void setVec_handler(char *msg){
    typename SetGet1CcsClient< A >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< A >::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = SetGet1<A>::setVec(Id(args.dest_.id), args.field_, args.values_);
    CcsSendReply(sizeof(bool), &ret);
  }


};

template<>
void SetGet1CcsServer<CcsId>::set_handler(char *msg){
  SetGet1CcsClient< CcsId >::Args args;
  CcsPackUnpack< SetGet1CcsClient< CcsId >::Args >::unpack(msg, args);
  CmiFree(msg);

  bool ret = SetGet1<Id>::set(ObjId(args.dest_), args.field_, Id(args.values_[0]));
  CcsSendReply(sizeof(bool), &ret);
}

template<>
void SetGet1CcsServer<CcsObjId>::set_handler(char *msg){
  SetGet1CcsClient< CcsObjId >::Args args;
  CcsPackUnpack< SetGet1CcsClient< CcsObjId >::Args >::unpack(msg, args);
  CmiFree(msg);

  bool ret = SetGet1<ObjId>::set(ObjId(args.dest_), args.field_, ObjId(args.values_[0]));
  CcsSendReply(sizeof(bool), &ret);
}



template<typename A>
class FieldCcsServer : public SetGet1CcsServer<A> {
  public:
  static void get_handler(char *msg){
    SetGetCcsClient::Args args;
    CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
    CmiFree(msg);

    A ret;
    bool success = Field<A>::get(ObjId(args.dest_), args.field_, ret); 
    // A might have members that need to be wrapper
    unsigned int size;
    SetGet1CcsWrapper< A > wrapper(ret, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< A > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }

  static void getVec_handler(char *msg){
    SetGetCcsClient::Args args; 
    CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
    CmiFree(msg);

    vector< A > ret;
    bool success = Field<A>::getVec(Id(args.dest_.id), args.field_, ret);

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
  CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
  CmiFree(msg);

  Id ret;
  bool success = Field<Id>::get(ObjId(args.dest_), args.field_, ret);

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
  CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
  CmiFree(msg);

  vector<Id> ret;
  bool success = Field<vector<Id> >::get(ObjId(args.dest_), args.field_, ret);

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
  CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
  CmiFree(msg);

  vector< Id > ret;
  bool success = Field<Id>::getVec(Id(args.dest_.id), args.field_, ret);

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
  CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
  CmiFree(msg);

  ObjId ret;
  bool success = Field<ObjId>::get(ObjId(args.dest_), args.field_, ret);

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
  CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
  CmiFree(msg);

  vector<ObjId> ret;
  bool success = Field<vector<ObjId> >::get(ObjId(args.dest_), args.field_, ret);

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
  CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
  CmiFree(msg);

  vector< ObjId > ret;
  bool success = Field<ObjId>::getVec(Id(args.dest_.id), args.field_, ret);

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
    CcsPackUnpack< typename SetGet2CcsClient<A1, A2>::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = SetGet2<A1, A2>::set(ObjId(args.dest_), args.field_, args.a1_[0], args.a2_[0]);
    CcsSendReply(sizeof(bool), &ret);
  }

  static void setVec_handler(char *msg){
    typename SetGet2CcsClient<A1, A2>::Args args;
    CcsPackUnpack< typename SetGet2CcsClient<A1, A2>::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = SetGet2<A1, A2>::setVec(ObjId(args.dest_), args.field_, args.a1_, args.a2_);
    CcsSendReply(sizeof(bool), &ret);
  }


};

template<typename L, typename A> 
class LookupFieldCcsServer : public SetGet2CcsServer<L, A> {
  public:
  static void get_handler(char *msg){
    typename SetGet1CcsClient< L >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< L >::Args >::unpack(msg, args);
    CmiFree(msg);

    A ret;
    bool success = LookupField<L, A>::get(ObjId(args.dest_), args.field_, args.values_[0], ret); 
    // A might have members that need to be packed
    unsigned int size;
    SetGet1CcsWrapper< A > wrapper(ret, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< A > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }

  static void getVec_handler(char *msg){
    typename SetGet1CcsClient< L >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< L >::Args >::unpack(msg, args);
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
    CcsPackUnpack< typename SetGet1CcsClient< L >::Args >::unpack(msg, args);
    CmiFree(msg);

    Id ret;
    bool success = LookupField<L, Id>::get(ObjId(args.dest_), args.field_, args.values_[0], ret); 

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
    CcsPackUnpack< SetGet1CcsClient< CcsId >::Args >::unpack(msg, args);
    CmiFree(msg);

    A ret;
    bool success = LookupField<Id, A>::get(ObjId(args.dest_), args.field_, Id(args.values_[0]), ret); 
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
    CcsPackUnpack< SetGet1CcsClient< CcsId >::Args >::unpack(msg, args);
    CmiFree(msg);

    Id ret;
    bool success = LookupField<Id, Id>::get(ObjId(args.dest_), args.field_, Id(args.values_[0]), ret); 
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
    CcsPackUnpack< SetGet1CcsClient< CcsId >::Args >::unpack(msg, args);
    CmiFree(msg);

    ObjId ret;
    bool success = LookupField<Id, ObjId>::get(ObjId(args.dest_), args.field_, Id(args.values_[0]), ret); 
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
    CcsPackUnpack< SetGet1CcsClient< CcsObjId >::Args >::unpack(msg, args);
    CmiFree(msg);

    Id ret;
    bool success = LookupField<ObjId, Id>::get(ObjId(args.dest_), args.field_, ObjId(args.values_[0]), ret); 
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
    CcsPackUnpack< typename SetGet1CcsClient< L >::Args >::unpack(msg, args);
    CmiFree(msg);

    ObjId ret;
    bool success = LookupField<L, ObjId>::get(ObjId(args.dest_), args.field_, args.values_[0], ret); 
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
    CcsPackUnpack< SetGet1CcsClient< CcsObjId >::Args >::unpack(msg, args);
    CmiFree(msg);

    A ret;
    bool success = LookupField<ObjId, A>::get(ObjId(args.dest_), args.field_, ObjId(args.values_[0]), ret); 
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
    CcsPackUnpack< SetGet1CcsClient< CcsObjId >::Args >::unpack(msg, args);
    CmiFree(msg);

    ObjId ret;
    bool success = LookupField<ObjId, ObjId>::get(ObjId(args.dest_), args.field_, ObjId(args.values_[0]), ret); 
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
    CcsPackUnpack< typename SetGet3CcsClient< A1, A2, A3 >::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = SetGet3<A1, A2, A3>::set(ObjId(args.dest_), args.field_, args.a1_, args.a2_, args.a3_);
    CcsSendReply(sizeof(bool), &ret);
  }
};


template<typename A1, typename A2, typename A3, typename A4> 
class SetGet4CcsServer : public SetGetCcsServer {
  static void set_handler(char *msg){
    typename SetGet4CcsClient< A1, A2, A3, A4 >::Args args;
    CcsPackUnpack< typename SetGet4CcsClient< A1, A2, A3, A4 >::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = SetGet4<A1, A2, A3, A4>::set(ObjId(args.dest_), args.field_, args.a1_, args.a2_, args.a3_, args.a4_);
    CcsSendReply(sizeof(bool), &ret);
  }
};


template< class A1, class A2, class A3, class A4, class A5 > class SetGet5CcsServer : public SetGetCcsServer {
  static void set_handler(char *msg){
    typename SetGet5CcsClient< A1, A2, A3, A4, A5 >::Args args;
    CcsPackUnpack< typename SetGet5CcsClient< A1, A2, A3, A4, A5 >::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = SetGet5<A1, A2, A3, A4, A5>::set(ObjId(args.dest_), args.field_, args.a1_, args.a2_, args.a3_, args.a4_, args.a5_);
    CcsSendReply(sizeof(bool), &ret);
  }
};


template< class A1, class A2, class A3, class A4, class A5, class A6 > class SetGet6CcsServer : public SetGetCcsServer {
  static void set_handler(char *msg){
    typename SetGet6CcsClient< A1, A2, A3, A4, A5, A6 >::Args args;
    CcsPackUnpack< typename::SetGet6CcsClient< A1, A2, A3, A4, A5, A6 >::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = SetGet6<A1, A2, A3, A4, A5, A6>::set(ObjId(args.dest_), args.field_, args.a1_, args.a2_, args.a3_, args.a4_, args.a5_, args.a6_);
    CcsSendReply(sizeof(bool), &ret);
  }
};

#endif // SET_GET_CCS_SERVER_H
