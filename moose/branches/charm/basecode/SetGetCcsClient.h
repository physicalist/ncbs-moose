#ifndef SET_GET_CCS_CLIENT_H
#define SET_GET_CCS_CLIENT_H

#include <string>
using namespace std;

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


  static bool strSet( const CcsObjId& dest, const string& field, const string& val ){
    unsigned int size;
    SetGet1CcsClient< string >::Args wrapper(tgt, field, v);
    char *msg = CcsPackUnpack< SetGet1CcsClient< string >::Args >::pack(wrapper, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, strSetHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    return ret;
  }

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

  static bool set_ccs( const CcsObjId& dest, const string& field ){
    unsigned int size;
    SetGetCcsClient::Args args(dest, field);
    char *msg = CcsPackUnpack<CcsSetGet::Args>::pack(args, size);
    CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, SetGet0CcsClient::setHandlerString().c_str(), size, msg);
    delete[] msg;

    bool ret;
    while(CcsRecvResponse(&CcsSetGet::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
    return false;
    // return ret;
  }

};

template<typename A> 
class SetGet1CcsClient : public SetGetCcsClient {
  static string setHandlerString_;
  static string setVecHandlerString_;

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
    char *msg = CcsPackUnpack< CcsSetGet::Args >::pack(wrapper, size);
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
    CcsSetGet::Args wrapper(CcsObjId(dest), field);
    char *msg = CcsPackUnpack< CcsSetGet::Args >::pack(wrapper, size);
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
    CcsPackUnpack< CcsSetGet1CcsWrapper< vector< A > > >::unpack(msg, ret);
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

  struct Args : public CcsSetGet::Args {
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


};

template<typename L, typename A>
string LookupFieldCcsClient<L, A>::getHandlerString_ = string("");
template<typename L, typename A>
string LookupFieldCcsClient<L, A>::getVecHandlerString_ = string("");

#endif // SET_GET_CCS_CLIENT_H
