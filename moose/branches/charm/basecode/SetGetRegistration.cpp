#include "charm++.h"
#include "pup_stl.h"
#include "header.h"
#include "../shell/ShellProxy.h"
#include "SetGetCcsClient.h"
#include "SetGetCcsServer.h"
#include "SetGetRegistration.h"

void registerSetGetCcsHandlers(map<string, string> &typeCodeTable, map<string, string> &classCodeTable){
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, string, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, unsigned int, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, bool, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, char, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, double, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, float, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, int, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, CcsId, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, CcsObjId, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<int>, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<double>, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<float>, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<unsigned int>, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<string>, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<CcsId>, get)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<CcsObjId>, get)

  MOOSE_SETGET_CCS_HANDLER_REG1(Field, bool, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, char, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, int, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, short, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, long, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, unsigned int, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, unsigned long, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, float, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, double, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, string, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, CcsId, getVec)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, CcsObjId, getVec)

  MOOSE_SETGET_CCS_HANDLER_REG1(Field, bool, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, char, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, short, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, int, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, long, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, unsigned int, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, unsigned long, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, float, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, double, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, string, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, CcsId, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, CcsObjId, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<int>, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<long>, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<short>, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<unsigned int>, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<unsigned long>, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<float>, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<double>, set)

  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<vector<unsigned> >, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<vector<int> >, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(Field, vector<vector<double> >, set)

  MOOSE_SETGET_CCS_HANDLER_REG2(LookupField, string, vector<CcsId>, get)

  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, bool, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, char, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, short, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, unsigned short, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, int, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, unsigned int, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, long, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, unsigned long, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, long long, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, unsigned long long, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, double, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, float, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, CcsId, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, CcsObjId, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, string, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, vector<int>, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, vector<short>, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, vector<unsigned int>, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, vector<float>, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, vector<double>, get)
  MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(LookupField, vector<string>, get)

  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, bool, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, char, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, short, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, unsigned short, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, int, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, unsigned int, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, long, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, unsigned long, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, long long, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, unsigned long long, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, double, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, float, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, CcsId, get)
  MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(LookupField, CcsObjId, get)

  // Needed in rtHHNetwork.cpp
  MOOSE_SETGET_CCS_HANDLER_REG1(SetGet1, double, set)
  MOOSE_SETGET_CCS_HANDLER_REG1(SetGet1, double, setVec)
  MOOSE_SETGET_CCS_HANDLER_REG2(SetGet2, double, long, set)
  MOOSE_SETGET_CCS_HANDLER_REG2(SetGet2, string, string, set)


  CcsRegisterHandler(SetGetCcsClient::strSetHandlerString().c_str(), (CmiHandler) SetGetCcsServer::strSet_handler);
  CcsRegisterHandler(SetGetCcsClient::strGetHandlerString().c_str(), (CmiHandler) SetGetCcsServer::strGet_handler);
  CcsRegisterHandler(SetGet0CcsClient::setHandlerString().c_str(), (CmiHandler) SetGet0CcsServer::set_handler);

}

#define MOOSE_SETGET_MERGE_1(T1, Method) \
CcsSetMergeFn(SetGet1CcsClient< T1 >::Method ## HandlerString().c_str(), CcsMerge_logical_and);

#define MOOSE_SETGET_MERGE_2(T1, T2, Method) \
CcsSetMergeFn(SetGet2CcsClient< T1, T2 >::Method ## HandlerString().c_str(), CcsMerge_logical_and);

#define MOOSE_SETGET_MERGE_1_OUTER(Method) \
MOOSE_SETGET_MERGE_1(bool, Method) \
MOOSE_SETGET_MERGE_1(char, Method) \
MOOSE_SETGET_MERGE_1(short, Method) \
MOOSE_SETGET_MERGE_1(unsigned short, Method) \
MOOSE_SETGET_MERGE_1(int, Method) \
MOOSE_SETGET_MERGE_1(unsigned int, Method) \
MOOSE_SETGET_MERGE_1(long, Method) \
MOOSE_SETGET_MERGE_1(unsigned long, Method) \
MOOSE_SETGET_MERGE_1(long long, Method) \
MOOSE_SETGET_MERGE_1(unsigned long long, Method) \
MOOSE_SETGET_MERGE_1(float, Method) \
MOOSE_SETGET_MERGE_1(double, Method) \
MOOSE_SETGET_MERGE_1(CcsId, Method) \
MOOSE_SETGET_MERGE_1(CcsObjId, Method) 

#define MOOSE_SETGET_MERGE_1_VECTOR_OUTER(Method) \
MOOSE_SETGET_MERGE_1(vector< bool >, Method) \
MOOSE_SETGET_MERGE_1(vector< char >, Method) \
MOOSE_SETGET_MERGE_1(vector< short >, Method) \
MOOSE_SETGET_MERGE_1(vector< unsigned short >, Method) \
MOOSE_SETGET_MERGE_1(vector< int >, Method) \
MOOSE_SETGET_MERGE_1(vector< unsigned int >, Method) \
MOOSE_SETGET_MERGE_1(vector< long >, Method) \
MOOSE_SETGET_MERGE_1(vector< unsigned long >, Method) \
MOOSE_SETGET_MERGE_1(vector< long long >, Method) \
MOOSE_SETGET_MERGE_1(vector< unsigned long long >, Method) \
MOOSE_SETGET_MERGE_1(vector< float >, Method) \
MOOSE_SETGET_MERGE_1(vector< double >, Method) \
MOOSE_SETGET_MERGE_1(vector< CcsId >, Method) \
MOOSE_SETGET_MERGE_1(vector< CcsObjId >, Method) 

#define MOOSE_SETGET_FIELD_MERGE(Type, Method) \
CcsSetMergeFn(FieldCcsClient< Type >::Method ## HandlerString().c_str(), (CmiReduceMergeFn) SetGetCcsServer::merge< SetGet1CcsWrapper< Type > > );

#define MOOSE_SETGET_FIELD_MERGE_OUTER(Method) \
MOOSE_SETGET_FIELD_MERGE(bool, Method) \
MOOSE_SETGET_FIELD_MERGE(char, Method) \
MOOSE_SETGET_FIELD_MERGE(short, Method) \
MOOSE_SETGET_FIELD_MERGE(unsigned short, Method) \
MOOSE_SETGET_FIELD_MERGE(int, Method) \
MOOSE_SETGET_FIELD_MERGE(unsigned int, Method) \
MOOSE_SETGET_FIELD_MERGE(long, Method) \
MOOSE_SETGET_FIELD_MERGE(unsigned long, Method) \
MOOSE_SETGET_FIELD_MERGE(long long, Method) \
MOOSE_SETGET_FIELD_MERGE(unsigned long long, Method) \
MOOSE_SETGET_FIELD_MERGE(float, Method) \
MOOSE_SETGET_FIELD_MERGE(double, Method) \
MOOSE_SETGET_FIELD_MERGE(CcsId, Method) \
MOOSE_SETGET_FIELD_MERGE(CcsObjId, Method) 

#define MOOSE_SETGET_FIELD_MERGE_VECTOR_OUTER(Method) \
MOOSE_SETGET_FIELD_MERGE(vector< bool >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< char >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< short >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< unsigned short >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< int >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< unsigned int >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< long >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< unsigned long >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< long long >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< unsigned long long >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< float >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< double >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< string >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< CcsId >, Method) \
MOOSE_SETGET_FIELD_MERGE(vector< CcsObjId >, Method) 

void registerSetGetMergeFns(map<string, string> &typeCodeTable, map<string, string> &classCodeTable){
  // SetGet0
  CcsSetMergeFn(SetGet0CcsClient::setHandlerString().c_str(), CcsMerge_logical_and);

  // SetGet1
  MOOSE_SETGET_MERGE_1_OUTER(set)
  MOOSE_SETGET_MERGE_1_VECTOR_OUTER(set)
  MOOSE_SETGET_MERGE_1_OUTER(setVec)
  MOOSE_SETGET_MERGE_1_VECTOR_OUTER(setVec)
  // put SetGet2, etc. merge functions below this line, as needed
  // For set/setVec, these should all be CcsMerge_logical_and
  // needed in rtHHNetwork
  MOOSE_SETGET_MERGE_1(double, setVec)
  MOOSE_SETGET_MERGE_2(double, long, set)
  MOOSE_SETGET_MERGE_2(string, string, set)

  // SetGet::strGet
  CcsSetMergeFn(SetGetCcsClient::strGetHandlerString().c_str(), (CmiReduceMergeFn) SetGetCcsServer::merge< SetGet1CcsWrapper< string > >);
  // Field::get/getVec 
  MOOSE_SETGET_FIELD_MERGE_OUTER(get)
  MOOSE_SETGET_FIELD_MERGE_VECTOR_OUTER(get)
  MOOSE_SETGET_FIELD_MERGE_OUTER(getVec)
   
  // put LookupField<> merge functions below this line, as needed
}

extern void initializeSetGetCcsHandlerStrings(map<string, string> &typeCodeTable, map<string, string> &classCodeTable);

void ccsRegistration(){
  map<string, string> typeCodeTable;
  map<string, string> classCodeTable;

  MooseSetClassCodes(classCodeTable);
  MooseSetTypeCodes(typeCodeTable);

  initializeSetGetCcsHandlerStrings(typeCodeTable, classCodeTable);
  registerSetGetCcsHandlers(typeCodeTable, classCodeTable);
  registerSetGetMergeFns(typeCodeTable, classCodeTable);
}
