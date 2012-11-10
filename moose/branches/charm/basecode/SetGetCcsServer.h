#ifndef SET_GET_CCS_SERVER_H
#define SET_GET_CCS_SERVER_H

#include <string>
using namespace std;

class SetGetCcsServer {
  public:
    static void strGet_handler(char *msg){
      SetGetCcsClient::Args args;
      CcsPackUnpack< SetGetCcsClient::Args >::unpack(msg, args);
      CmiFree(msg);

      string ret;
      bool success = strGet(args.dest_, args.field_, ret);
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

      bool ret = strSet(args.dest_, args.field_, args.values_[0]); 
      CcsSendReply(sizeof(bool), &ret);
    }
};

class SetGet0CcsServer {
  public:
  static void set_handler(char *msg){
    SetGetCcsClient::Args args;
    CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
    CmiFree(msg);

    bool ret = set(args.dest_, args.field_);
    CcsSendReply(sizeof(bool), &ret);
  }
};

template<typename A>
class SetGet1CcsServer : public SetGetCcsServer {
  public:
  static void set_handler(char *msg){
    SetGet1CcsClient< A >::Args args;
    CcsPackUnpack< SetGet1CcsClient< A >::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = set(args.dest_, args.field_, args.values_[0]);
    CcsSendReply(sizeof(bool), &ret);
  }

  static void setVec_handler(char *msg){
    SetGet1CcsClient< A >::Args args;
    CcsPackUnpack< CcsSetGet1< A >::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = setVec(args.dest_.id, args.field_, args.values_);
    CcsSendReply(sizeof(bool), &ret);
  }


};

template<typename A>
class FieldCcsServer : public SetGetCcsServer<A> {
  public:
  static void get_handler(char *msg){
    SetGetCcsClient::Args args;
    CcsPackUnpack<SetGetCcsClient::Args>::unpack(msg, args);
    CmiFree(msg);

    A ret;
    bool success = get(args.dest_, args.field_, ret); 
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
    bool success = getVec(args.dest_.id, args.field_, ret);

    // A might have members that need to be wrapper
    unsigned int size;
    SetGet1CcsWrapper< vector< A > > wrapper(ret, success);
    msg = CcsPackUnpack< SetGet1CcsWrapper< vector< A > > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }


};

template<typename A1, typename A2> 
class SetGet2CcsServer : public SetGetCcsServer {
  public:
  static void set_handler(char *msg){
    SetGet2CcsClient::Args args;
    CcsPackUnpack< SetGet2CcsClient::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = set(args.dest_, args.field_, args.a1_[0], args.a2_[0]);
    CcsSendReply(sizeof(bool), &ret);
  }

  static void setVec_handler(char *msg){
    SetGet2CcsClient::Args args;
    CcsPackUnpack< SetGet2CcsClient::Args >::unpack(msg, args);
    CmiFree(msg);

    bool ret = setVec(args.dest_, args.field_, args.a1_, args.a2_);
    CcsSendReply(sizeof(bool), &ret);
  }


};

template<typename L, typename A> 
class LookupFieldCcsServer : public SetGet2CcsServer<L, A> {
  public:
  static void get_handler(char *msg){
    typename SetGet1CcsClient< A >::Args args;
    CcsPackUnpack< typename SetGet1CcsClient< A >::Args >::unpack(msg, args);
    CmiFree(msg);

    A ret;
    bool success = get(args.dest_, args.field_, args.values_[0], ret); 
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
    getVec(args.dest_.id, args.field_, args.values_, ret); 
    // A might have members that need to be packed
    unsigned int size;
    SetGet1CcsWrapper< vector< A > > wrapper(ret, true);
    msg = CcsPackUnpack< SetGet1CcsWrapper< vector< A > > >::pack(wrapper, size);

    CcsSendReply(size, msg);
    delete[] msg;
  }
};

#endif // SET_GET_CCS_SERVER_H
