#include "header.h"
#include "SetGet.h"

// SetGetCcsClient.h contains a bunch of template classes that
// are meant for fetching typed data from objects. When
// using CCS, we must assign a unique string to the 
// handler for each type. These strings are static 
// members of the SetGet/Field/LookupField template
// classes. The parser instantiates set/get calls at
// various points. We use these instantiations to see
// which set/get instances will be used in the program, 
// and initialize the corresponding handler strings here

// these are needed for ID_GETVEC
#define MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Class, Type, Method) \
Class< Type >::setHandlerString(std::string("Moose::"#Class"< "#Type" >::"#Method"Handler"));

// these are needed for lookup_value
#define MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, Type, Method)\
Class< KeyType, Type >::setHandlerString(std::string("Moose::"#Class"< "#KeyType", "#Type" >::"#Method"Handler"));

// for each KeyType, generates definitions for all ValueTypes
#define MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(Class, KeyType, Method)\
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
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, Id, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, ObjId, Method)

// this macro is reused for set and get
#define MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER_LookupField_METHOD(Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, bool, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, char, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, short, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, unsigned short, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, int, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, unsigned int, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, unsigned long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, long long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, unsigned long long, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, double, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, float, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, string, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, Id, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, ObjId, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, vector< int >, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, vector< short >, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, vector< unsigned int >, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, vector< float >, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, vector< double >, Method)\
MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER(LookupFieldCcsClient, vector< string >, Method)

void initializeSetGetCcsHandlerStrings(){
  // these are needed for ID_GETVEC
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, char, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, int, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, short, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, long, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, unsigned int, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, unsigned long, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, float, getVec)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, double, getVec)

  // these are needed for GET_FIELD
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, char, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, int, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, short, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, long, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, unsigned int, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, unsigned long, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, float, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, double, get)

  // these are needed for GET_VECFIELD
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< double >, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< float >, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< int >, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< long >, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< unsigned long >, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< unsigned int >, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< short >, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< char >, get)

  // these are needed for GET_VECVEC
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< vector < unsigned int > >, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< vector < int > >, get)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(FieldCcsClient, vector< vector < double > >, get)

  // these are needed for _set_vector_destFinfo
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(SetGet1CcsClient, vector < int >, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(SetGet1CcsClient, vector < short >, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(SetGet1CcsClient, vector < unsigned long >, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(SetGet1CcsClient, vector < float >, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(SetGet1CcsClient, vector < double >, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(SetGet1CcsClient, vector < string >, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(SetGet1CcsClient, vector < Id >, set)
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(SetGet1CcsClient, vector < ObjId >, set)

  // these are for lookup_value
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER_LookupField_METHOD(get)
  // these are for set_lookup_value
  MOOSE_SETGET_CCS_HANDLER_STRING_DEF2_OUTER_LookupField_METHOD(set)
}
