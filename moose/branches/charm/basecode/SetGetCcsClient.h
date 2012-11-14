#ifndef SET_GET_CCS_CLIENT_H
#define SET_GET_CCS_CLIENT_H

#include <string>
using namespace std;

#include "ccs-client.h"
#include "SetGetWrapper.h"
#include "CcsPackUnpack.h"
#include "CcsId.h" 
#include "CcsObjId.h" 
#include "CcsDataId.h" 
#include "../shell/ShellProxy.h"
#include "pup_stl.h"

class SetGetCcsClient {
  // to be inherited by all derived classes
  public:
  static CcsServer ccsServer_;

  private:
  static string strSetHandlerString_;
  static string strGetHandlerString_;

  public:
  static string strSetHandlerString(){
    return strSetHandlerString_;
  }

  static void strSetHandlerString(const string &str){
    strSetHandlerString_ = str;
  }

  static string strGetHandlerString(){
    return strGetHandlerString_;
  }

  static void strGetHandlerString(const string &str){
    strGetHandlerString_ = str;
  }

  static bool strGet( const CcsObjId& tgt, const string& field, string& ret ){
    unsigned int size;
    SetGetCcsClient::Args wrapper(tgt, field);
    char *msg = CcsPackUnpack< SetGetCcsClient::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, strGetHandlerString().c_str(), size, msg);
    delete[] msg;

    SetGet1CcsWrapper< string > sg;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);

    ret = sg.a1_;
    return sg.hasData_;
  }


  // this is defined later, owing to its use of SetGet1CcsClient<A>::Args
  static bool strSet( const CcsObjId& dest, const string& field, const string& val );

  struct Args {
    CcsObjId dest_;
    string field_;

    Args(CcsObjId dest, string field) : 
      dest_(dest),
      field_(field)
    {}

    Args() {}

    void pup(PUP::er &p){
      p | dest_;
      p | field_;
    }
  };

  public:
  static void connect(string &name, int port){
    CcsConnect(&ccsServer_, name.c_str(), port, NULL);
  }

};

class SetGet0CcsClient : public SetGetCcsClient {
  static string setHandlerString_;

  public:
  static string setHandlerString(){
    return setHandlerString_;
  }

  static void setHandlerString(const string &str){
    setHandlerString_ = str;
  }

  static bool set( const CcsObjId& dest, const string& field ){
    unsigned int size;
    SetGetCcsClient::Args args(dest, field);
    char *msg = CcsPackUnpack<SetGetCcsClient::Args>::pack(args, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, SetGet0CcsClient::setHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    return false;
    // return ret;
  }

};

template<typename A> 
class SetGet1CcsClient : public SetGetCcsClient {
  static string setHandlerString_;
  static string setVecHandlerString_;

  public:
  static void setHandlerString(const string &str){
    setHandlerString_ = str;
  }

  static string setHandlerString(){
    return setHandlerString_;
  }

  static void setVecHandlerString(const string &str){
    setVecHandlerString_ = str;
  }

  static string setVecHandlerString(){
    return setVecHandlerString_;
  }


  static bool set(const CcsObjId& dest, const string& field, A arg){
    unsigned int size;
    SetGet1CcsClient< A >::Args wrapper(dest, field, arg);
    char *msg = CcsPackUnpack< SetGet1CcsClient< A >::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, SetGet1CcsClient< A >::setHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    // original version always returned 'false'
    return false;
    // return ret;
  }

  static bool setVec(const CcsId dest, const string &field, const vector<A> &arg){
    unsigned int size;
    SetGet1CcsClient< A >::Args wrapper(CcsObjId(dest), field, arg);
    char *msg = CcsPackUnpack< SetGet1CcsClient< A >::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, SetGet1CcsClient::setVecHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    // original version always returned 'false'
    return false;
    // return ret;
  }

  static bool setRepeat( CcsId destId, const string& field, 
      const A& arg )
  {
    vector< A >temp ( 1, arg );
    return setVec( destId, field, temp );
  }

  struct Args : public SetGetCcsClient::Args {
    vector<A> values_;

    Args(const CcsObjId &objId, string field, const A &value) : 
      SetGetCcsClient::Args(objId, field)
    {
      values_.push_back(value);
    }

    Args(const CcsObjId &objId, string field, const vector<A> &values) : 
      SetGetCcsClient::Args(objId, field),
      values_(values)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGetCcsClient::Args::pup(p);
      p | values_; 
    }
  };
};

template<typename A>
string SetGet1CcsClient<A>::setHandlerString_ = string("");
template<typename A>
string SetGet1CcsClient<A>::setVecHandlerString_ = string("");


template<typename A>
class FieldCcsClient : public SetGet1CcsClient<A> {
  static string getHandlerString_;
  static string getVecHandlerString_;
  static string setHandlerString_;
  static string setVecHandlerString_;

  public:

  static void getHandlerString(const string &str){
    getHandlerString_ = str;
  }

  static string getHandlerString(){
    return getHandlerString_;
  }

  static void getVecHandlerString(const string &str){
    getVecHandlerString_ = str;
  }

  static string getVecHandlerString(){
    return getVecHandlerString_;
  }

  static string setVecHandlerString(){
    return setVecHandlerString_;
  }

  static void setVecHandlerString(const string &str){
    setVecHandlerString_ = str;
  }

  static bool set( const CcsObjId& dest, const string& field, A arg )
  {
    string temp = "set_" + field;
    return SetGet1CcsClient< A >::set( dest, temp, arg );
  }


  static A get( const CcsObjId& dest, const string& field, bool sendToSingleNode = false){
    unsigned int size;
    SetGetCcsClient::Args wrapper(dest, field);
    char *msg = CcsPackUnpack< SetGetCcsClient::Args >::pack(wrapper, size);
    if(sendToSingleNode){
      CcsSendRequest(&SetGetCcsClient::ccsServer_, FieldCcsClient< A >::getHandlerString().c_str(), 0, size, msg);
    }
    else{
      CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, FieldCcsClient< A >::getHandlerString().c_str(), size, msg);
    }
    delete[] msg;

    int msgSize;
    // don't know how much data to receive,
    // so receive the whole msg first and 
    // unpack the data within it
    while(CcsRecvResponseMsg(&SetGetCcsClient::ccsServer_, &msgSize, (void **) &msg, MOOSE_CCS_TIMEOUT) <= 0);
    // type A might need to be unwrapper
    SetGet1CcsWrapper< A > ret;
    CcsPackUnpack< SetGet1CcsWrapper< A > >::unpack(msg, ret);
    // msg will point to a malloc()ed buffer after CcsRecvResponseMsg
    free(msg);
    CkAssert(ret.hasData_);
    return ret.a1_;
  }

  static void getVec( CcsId dest, const string& field, vector< A >& vec, bool sendToSingleNode = true){
    unsigned int size;
    SetGetCcsClient::Args wrapper(CcsObjId(dest), field);
    char *msg = CcsPackUnpack< SetGetCcsClient::Args >::pack(wrapper, size);
    if(sendToSingleNode){
      CcsSendRequest(&SetGetCcsClient::ccsServer_, FieldCcsClient< A >::getVecHandlerString().c_str(), 0, size, msg);
    }
    else{
      CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, FieldCcsClient< A >::getVecHandlerString().c_str(), size, msg);
    }
    delete[] msg;

    int replySize;
    while(CcsRecvResponseMsg(&SetGetCcsClient::ccsServer_, &replySize, (void **) &msg, MOOSE_CCS_TIMEOUT) <= 0); 
    SetGet1CcsWrapper< vector<A> > ret;
    CcsPackUnpack< SetGet1CcsWrapper< vector< A > > >::unpack(msg, ret);
    free(msg);

    vec = ret.a1_;
  }

};

template<typename A>
string FieldCcsClient<A>::getHandlerString_ = string("");
template<typename A>
string FieldCcsClient<A>::getVecHandlerString_ = string("");
template<typename A>
string FieldCcsClient<A>::setHandlerString_ = string("");
template<typename A>
string FieldCcsClient<A>::setVecHandlerString_ = string("");

template<typename A1, typename A2>
class SetGet2CcsClient : public SetGetCcsClient {
  static string setHandlerString_;
  static string setVecHandlerString_;

  public:
  static void setHandlerString(const string &str){
    setHandlerString_ = str;
  }

  static string setHandlerString(){
    return setHandlerString_;
  }

  static void setVecHandlerString(const string &str){
    setVecHandlerString_ = str;
  }

  static string setVecHandlerString(){
    return setVecHandlerString_;
  }

  static bool set( const CcsObjId& dest, const string& field, A1 arg1, A2 arg2 ){
    unsigned int size;
    SetGet2CcsClient::Args wrapper(dest, field, arg1, arg2);
    char *msg = CcsPackUnpack< SetGet2CcsClient::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, SetGet2CcsClient::setHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    return ret;
  }

  static bool setVec( CcsId destId, const string& field, const vector< A1 >& arg1, const vector< A2 >& arg2 ){
    unsigned int size;
    Args wrapper(CcsObjId(destId), field, arg1, arg2);
    char *msg = CcsPackUnpack< Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, SetGet2CcsClient::setVecHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    return ret;
  }

  struct Args : public SetGetCcsClient::Args {
    vector< A1 > a1_;
    vector< A2 > a2_;

    Args(const CcsObjId &objId, string field, A1 a1, A2 a2) : 
      SetGetCcsClient::Args(objId, field) 
    {
      a1_.push_back(a1);
      a2_.push_back(a2);
    }

    Args(const CcsObjId &objId, string field, const vector< A1 > &a1, const vector< A2 > &a2) : 
      SetGetCcsClient::Args(objId, field),
      a1_(a1),
      a2_(a2)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGetCcsClient::Args::pup(p);
      p | a1_;
      p | a2_;
    }
  };
};

template<typename A1, typename A2>
string SetGet2CcsClient<A1, A2>::setHandlerString_ = string("");
template<typename A1, typename A2>
string SetGet2CcsClient<A1, A2>::setVecHandlerString_ = string("");

template<typename L, typename A>
class LookupFieldCcsClient : public SetGet2CcsClient<L, A> {
  static string getHandlerString_;
  static string getVecHandlerString_;

  public:
  static void getHandlerString(const string &str){
    getHandlerString_ = str;
  }

  static string getHandlerString(){
    return getHandlerString_;
  }

  static void getVecHandlerString(const string &str){
    getVecHandlerString_ = str;
  }

  static string getVecHandlerString(){
    return getVecHandlerString_;
  }

  static A get(const CcsObjId& dest, const string& field, L index){
    unsigned int size;
    typename SetGet1CcsClient< L >::Args wrapper(dest, field, index);
    char *msg = CcsPackUnpack< typename SetGet1CcsClient< L >::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, LookupFieldCcsClient< L, A >::getHandlerString().c_str(), size, msg);
    delete[] msg;

    int msgSize;
    while(CcsRecvResponseMsg(&SetGetCcsClient::ccsServer_, &msgSize, (void **) &msg, MOOSE_CCS_TIMEOUT) <= 0);
    // types L, A might need to be unwrapper
    SetGet1CcsWrapper< A > ret;
    CcsPackUnpack< SetGet1CcsWrapper< A > >::unpack(msg, ret);
    // msg will point to a malloc()ed buffer after CcsRecvResponseMsg
    free(msg);
    return ret.a1_;
  }

  static void getVec(const CcsId& dest, const string& field, const vector< L > &index, vector< A > &vec){
    unsigned int size;
    typename SetGet1CcsClient< L >::Args wrapper(CcsObjId(dest), field, index);
    char *msg = CcsPackUnpack< typename SetGet1CcsClient< L >::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, LookupFieldCcsClient< L, A >::getVecHandlerString().c_str(), size, msg);
    delete[] msg;

    int msgSize;
    while(CcsRecvResponseMsg(&SetGetCcsClient::ccsServer_, &msgSize, (void **) &msg, MOOSE_CCS_TIMEOUT) <= 0);
    // types L, A might need to be unwrapper
    SetGet1CcsWrapper< A > ret;
    CcsPackUnpack< SetGet1CcsWrapper< A > >::unpack(msg, ret);
    // msg will point to a malloc()ed buffer after CcsRecvResponseMsg
    free(msg);
    vec = ret.a1_;
  }

  static bool set( const CcsObjId& dest, const string& field, L index, A arg ){
    string temp = "set_" + field;
    return SetGet2CcsClient< L, A >::set( dest, temp, index, arg );
  }

  /** 
   * This setVec assigns goes through each object entry in the
   * destId, and assigns the corresponding index and argument to it.
   */
  static bool setVec( CcsId destId, const string& field, const vector< L >& index, const vector< A >& arg ){
    string temp = "set_" + field;
    return SetGet2CcsClient< L, A >::setVec( destId, temp, index, arg );
  }

  /**
   * This setVec takes a specific object entry, presumably one with
   * an array of values within it. The it goes through each specified
   * index and assigns the corresponding argument.
   * This is a brute-force assignment.
   */
  static bool setVec( CcsObjId dest, const string& field, const vector< L >& index, const vector< A >& arg ){
    string temp = "set_" + field;
    return SetGet2CcsClient< L, A >::setVec( dest, temp, index, arg );
  }

};

template<typename L, typename A>
string LookupFieldCcsClient<L, A>::getHandlerString_ = string("");
template<typename L, typename A>
string LookupFieldCcsClient<L, A>::getVecHandlerString_ = string("");

template<typename A1, typename A2, typename A3> 
class SetGet3CcsClient : public SetGetCcsClient {
  static string setHandlerString_;

  public:
  static void setHandlerString(const string &str){
    setHandlerString_ = str;
  }

  static string setHandlerString(){
    return setHandlerString_;
  }

  struct Args : public SetGetCcsClient::Args {
    A1 a1_;
    A2 a2_;
    A3 a3_;

    Args(CcsObjId dest, string field, A1 a1, A2 a2, A3 a3) :
      SetGetCcsClient::Args(dest, field),
      a1_(a1),
      a2_(a2),
      a3_(a3)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGetCcsClient::Args::pup(p);
      p | a1_;
      p | a2_;
      p | a3_;
    }
  };

  static bool set( const CcsObjId& dest, const string& field, 
      A1 arg1, A2 arg2, A3 arg3 ){
    unsigned int size;
    SetGet3CcsClient::Args wrapper(dest, field, arg1, arg2, arg3);
    char *msg = CcsPackUnpack< SetGet3CcsClient::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, SetGet3CcsClient::setHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    return ret;
  }
};

template<typename A1, typename A2, typename A3>
string SetGet3CcsClient<A1, A2, A3>::setHandlerString_ = string("");

template<typename A1, typename A2, typename A3, typename A4> 
class SetGet4CcsClient : public SetGetCcsClient {
  static string setHandlerString_;

  public:
  static void setHandlerString(const string &str){
    setHandlerString_ = str;
  }

  static string setHandlerString(){
    return setHandlerString_;
  }

  struct Args : public SetGetCcsClient::Args {
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;

    Args(CcsObjId dest, string field, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4) :
      SetGetCcsClient::Args(dest, field),
      a1_(a1),
      a2_(a2),
      a3_(a3),
      a4_(a4)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGetCcsClient::Args::pup(p);
      p | a1_;
      p | a2_;
      p | a3_;
      p | a4_;
    }
  };

  static bool set( const CcsObjId& dest, const string& field, A1 arg1, A2 arg2, A3 arg3, A4 arg4){
    unsigned int size;
    SetGet4CcsClient::Args wrapper(dest, field, arg1, arg2, arg3, arg4);
    char *msg = CcsPackUnpack< SetGet4CcsClient::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, SetGet4CcsClient::setHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    return ret;
  }
};

template<typename A1, typename A2, typename A3, typename A4>
string SetGet4CcsClient<A1, A2, A3, A4>::setHandlerString_ = string("");


template< class A1, class A2, class A3, class A4, class A5 > 
class SetGet5CcsClient : public SetGetCcsClient {
  static string setHandlerString_;

  public:
  static void setHandlerString(const string str){
    setHandlerString_ = str;
  }

  static string setHandlerString(){
    return setHandlerString_;
  }

  struct Args : public SetGetCcsClient::Args {
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;

    Args(CcsObjId dest, string field, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5) :
      SetGetCcsClient::Args(dest, field),
      a1_(a1),
      a2_(a2),
      a3_(a3),
      a4_(a4),
      a5_(a5)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGetCcsClient::Args::pup(p);
      p | a1_;
      p | a2_;
      p | a3_;
      p | a4_;
      p | a5_;
    }
  };

  static bool set( const CcsObjId& dest, const string& field, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5){
    unsigned int size;
    SetGet5CcsClient::Args wrapper(dest, field, arg1, arg2, arg3, arg4, arg5);
    char *msg = CcsPackUnpack< SetGet5CcsClient::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, SetGet5CcsClient::setHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    return ret;
  }



};

template<typename A1, typename A2, typename A3, typename A4, typename A5>
string SetGet5CcsClient<A1, A2, A3, A4, A5>::setHandlerString_ = string("");

template< class A1, class A2, class A3, class A4, class A5, class A6 > class SetGet6CcsClient : public SetGetCcsClient {
  static string setHandlerString_;

  public:
  static void setHandlerString(const string &str){
    setHandlerString_ = str;
  }

  static string setHandlerString(){
    return setHandlerString_;
  }

  struct Args : public SetGetCcsClient::Args {
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
    A6 a6_;

    Args(CcsObjId dest, string field, 
        const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4, const A5 &a5, const A6 &a6) :
      SetGetCcsClient::Args(dest, field),
      a1_(a1),
      a2_(a2),
      a3_(a3),
      a4_(a4),
      a5_(a5),
      a6_(a6)
    {}

    Args() {}

    void pup(PUP::er &p){
      SetGetCcsClient::Args::pup(p);
      p | a1_;
      p | a2_;
      p | a3_;
      p | a4_;
      p | a5_;
      p | a6_;
    }
  };

  static bool set( const CcsObjId& dest, const string& field, A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6){
    unsigned int size;
    SetGet6CcsClient::Args wrapper(dest, field, arg1, arg2, arg3, arg4, arg5, arg6);
    char *msg = CcsPackUnpack< SetGet6CcsClient::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, SetGet6CcsClient::setHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    return ret;
  }
};

template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
string SetGet6CcsClient<A1, A2, A3, A4, A5, A6>::setHandlerString_ = string("");



#endif // SET_GET_CCS_CLIENT_H
