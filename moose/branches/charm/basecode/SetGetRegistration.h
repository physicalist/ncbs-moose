#ifndef MOOSE_SET_GET_REGISTRATION_H
#define MOOSE_SET_GET_REGISTRATION_H

#include <string>
#include <vector>
#include <map>

using namespace std;

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

extern void MooseSetTypeCodes(map<string, string> &table);
extern void MooseSetClassCodes(map<string, string> &table);
extern string MooseGetTypeCode(map<string, string> &table, string in);

// **********************************
// These are to set static string
// descriptors of methods and to reg.
// handler methods themselves
// **********************************
// set static string + register handler for 'field' methods
#define MOOSE_SETGET_CCS_HANDLER_STRING_DEF1(Class, Type, Method) \
Class ## CcsClient< Type >::Method ## HandlerString(std::string(MooseGetTypeCode(classCodeTable, #Class) + "<" + MooseGetTypeCode(typeCodeTable, #Type) + ">::"#Method));

// set static string + register handler for 'lookupfield' methods
#define MOOSE_SETGET_CCS_HANDLER_STRING_DEF2(Class, KeyType, Type, Method)\
Class ## CcsClient< KeyType, Type >::Method ## HandlerString(std::string(MooseGetTypeCode(classCodeTable, #Class) + "<" + MooseGetTypeCode(typeCodeTable, #KeyType) + "," + MooseGetTypeCode(typeCodeTable, #Type) + ">::"#Method));

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

#define MOOSE_SETGET_CCS_HANDLER_REG2_OUTER(Class, KeyType, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, bool, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, char, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, short, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, int, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, long, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, unsigned long, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, long long, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, unsigned long long, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, double, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, float, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, CcsId, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, CcsObjId, Method)

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

#define MOOSE_SETGET_CCS_HANDLER_REG3_OUTER(Class, KeyType, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, bool, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, char, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, short, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, unsigned short, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, int, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, unsigned int, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, long, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, unsigned long, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, long long, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, unsigned long long, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, double, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, float, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, string, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, CcsId, Method)\
MOOSE_SETGET_CCS_HANDLER_REG2(Class, KeyType, CcsObjId, Method)

#endif // MOOSE_SET_GET_REGISTRATION_H
