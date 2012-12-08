// STANDALONE causes CmiAbort, etc. to be defined.
// this is required since we will be linking with g++,
// not charmc
#define STANDALONE_PUP
#include "sundry.h"
#include "converse.h"
#include "SetGetCcsClient.h"
#undef STANDALONE_PUP

CcsServer SetGetCcsClient::ccsServer_;

string SetGetCcsClient::strSetHandlerString_ = string("");
string SetGetCcsClient::strGetHandlerString_ = string("");

string SetGet0CcsClient::setHandlerString_ = string("");

// static strings belonging to templated classes are
// instantiated where they are used

bool SetGetCcsClient::strSet( const CcsObjId& dest, const string& field, const string& val ){
  unsigned int size;
  SetGet1CcsClient< string >::Args wrapper(dest, field, val);
  char *msg = CcsPackUnpack< SetGet1CcsClient< string >::Args >::pack(wrapper, size);
  CcsSendBroadcastRequest(&SetGetCcsClient::ccsServer_, strSetHandlerString().c_str(), size, msg);
  delete[] msg;

  bool ret;
  while(CcsRecvResponse(&SetGetCcsClient::ccsServer_, sizeof(bool), &ret, MOOSE_CCS_TIMEOUT) <= 0);
  return ret;
}


