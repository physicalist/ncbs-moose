#include "converse.h"
#include "pup_stl.h"
#include "header.h"
#include "../shell/ShellProxy.h"
#include "SetGetCcsClient.h"
#include "SetGetCcsServer.h"

// SetGetCcsClient.h contains a bunch of template classes that
// are meant for fetching typed data from objects. When
// using CCS, we must assign a unique string to the 
// handler for each type. These strings are static 
// members of the SetGet/Field/LookupField template
// classes. The parser instantiates set/get calls at
// various points. We use these instantiations to see
// which set/get instances will be used in the program, 
// and initialize the corresponding handler strings here

// **********************************
// These are for handler registration
// **********************************
// field method registration
#define MOOSE_SETGET_CCS_HANDLER_REG1(Class, Type, Method) \
CcsRegisterHandler(Class ## CcsClient< Type >::Method ## HandlerString().c_str(), (CmiHandler) Class ## CcsServer< Type >::Method ## _handler);

// lookupfield static method registration
#define MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, Type, Method)\
CcsRegisterHandler(Class ## CcsClient< KeyType, Type >::Method ## HandlerString().c_str(), (CmiHandler) Class ## CcsServer< KeyType, Type >::Method ## _handler);

// **********************************
// These are to set static string
// descriptors of methods and to reg.
// handler methods themselves
// **********************************
// set static string + register handler for 'field' methods
#define MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Class, Type, Method) \
Class ## CcsClient< Type >::Method ## HandlerString(std::string("Moose::"#Class"CcsClient< "#Type" >::"#Method"Handler"));\
MOOSE_SETGET_CCS_HANDLER_REG1(Class, Type, Method)

// set static string + register handler for 'lookupfield' methods
#define MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, Type, Method)\
Class ## CcsClient< KeyType, Type >::Method ## HandlerString(std::string("Moose::"#Class"CcsClient< "#KeyType", "#Type" >::"#Method"Handler"));\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, Type, Method)

// for each KeyType, generates definitions for all ValueTypes
#define MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(Class, KeyType, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, bool, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, char, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, short, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, int, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, unsigned long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, long long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, unsigned long long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, double, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, float, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, CcsId, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, CcsObjId, Method)

// for set_lookup_value; this one has a few more types than the above
#define MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(Class, KeyType, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, bool, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, char, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, short, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, unsigned short, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, int, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, unsigned int, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, unsigned long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, long long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, unsigned long long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, double, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, float, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, string, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, CcsId, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, CcsObjId, Method)

void initializeSetGetCcsHandlerStrings(){
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, string, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, unsigned int, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, bool, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, char, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, double, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, float, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, int, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, CcsId, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, CcsObjId, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<int>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<double>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<float>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<unsigned int>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<string>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<CcsId>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<CcsObjId>, get)

  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, bool, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, char, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, int, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, short, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, long, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, unsigned int, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, unsigned long, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, float, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, double, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, string, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, CcsId, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, CcsObjId, getVec)

  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, bool, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, char, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, short, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, int, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, long, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, unsigned int, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, unsigned long, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, float, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, double, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, string, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, CcsId, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, CcsObjId, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<int>, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<long>, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<short>, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<unsigned int>, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<unsigned long>, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<float>, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<double>, set)

  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<vector<unsigned> >, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<vector<int> >, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Field, vector<vector<double> >, set)

  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(LookupField, string, vector<CcsId>, get)

  // these are for lookup_value
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, bool, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, char, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, short, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, unsigned short, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, int, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, unsigned int, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, long, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, unsigned long, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, long long, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, unsigned long long, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, double, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, float, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, CcsId, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, CcsObjId, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, string, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, vector<int>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, vector<short>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, vector<unsigned int>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, vector<float>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, vector<double>, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupField, vector<string>, get)

  // these are for set_lookup_value
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, bool, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, char, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, short, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, unsigned short, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, int, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, unsigned int, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, long, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, unsigned long, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, long long, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, unsigned long long, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, double, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, float, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, CcsId, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF3_OUTER(LookupField, CcsObjId, get)

  CcsRegisterHandler(SetGetCcsClient::strSetHandlerString().c_str(), (CmiHandler) SetGetCcsServer::strSet_handler);
  CcsRegisterHandler(SetGetCcsClient::strGetHandlerString().c_str(), (CmiHandler) SetGetCcsServer::strGet_handler);
  CcsRegisterHandler(SetGet0CcsClient::setHandlerString().c_str(), (CmiHandler) SetGet0CcsServer::set_handler);
}

void registerSetGetCcsHandlers(){
}

void ccsRegistration(){
  initializeSetGetCcsHandlerStrings();
  registerSetGetCcsHandlers();
}
