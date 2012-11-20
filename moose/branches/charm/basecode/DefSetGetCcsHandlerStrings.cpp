#include "charm++.h"
#include "SetGetRegistration.h"
#include "SetGetCcsClient.h"

void MooseSetTypeCodes(map<string, string> &table){
  table["bool"              ] = string("b");
  table["char"              ] = string("c");
  table["short"             ] = string("h");
  table["unsigned short"    ] = string("H");
  table["int"               ] = string("i");
  table["unsigned int"      ] = string("u");
  table["long"              ] = string("l");
  table["unsigned long"     ] = string("m");
  table["long long"         ] = string("L");
  table["unsigned long long"] = string("M");
  table["double"            ] = string("d");
  table["float"             ] = string("f");
  table["string"            ] = string("s");
  table["CcsId"             ] = string("I");
  table["CcsDataId"         ] = string("D");
  table["CcsObjId"          ] = string("O");

  table["vector<bool>"              ] = string("v<b>");
  table["vector<char>"              ] = string("v<c>");
  table["vector<short>"             ] = string("v<h>");
  table["vector<int>"               ] = string("v<i>");
  table["vector<unsigned int>"      ] = string("v<u>");
  table["vector<long>"              ] = string("v<l>");
  table["vector<unsigned long>"     ] = string("v<m>");
  table["vector<long long>"         ] = string("v<L>");
  table["vector<unsigned long long>"] = string("v<M>");
  table["vector<double>"            ] = string("v<d>");
  table["vector<float>"             ] = string("v<f>");
  table["vector<string>"            ] = string("v<s>");
  table["vector<CcsId>"             ] = string("v<I>");
  table["vector<CcsDataId>"         ] = string("v<D>");
  table["vector<CcsObjId>"          ] = string("v<O>");

  table["vector<vector<unsigned> >"] = string("v<v<b>>");
  table["vector<vector<int> >"     ] = string("v<v<i>>");
  table["vector<vector<double> >"  ] = string("v<v<d>>");

  /*
  map<string, string>::iterator it;
  for(it = table.begin(); it != table.end(); ++it){
    CkPrintf("types[%s] = %s\n", it->first.c_str(), it->second.c_str());
  }
  */
}

void MooseSetClassCodes(map<string, string> &table){
  table["Field"      ] = string("F"); 
  table["LookupField"] = string("LF"); 
}

string MooseGetTypeCode(map<string, string> &table, string in){
  map<string, string>::iterator it;
  it = table.find(in);
  if(it == table.end()){
    CkPrintf("MooseGetTypeCode unknown type: %s\n", in.c_str());
    CkAbort("bad type passed to MooseGetTypeCode\n");
  }

  return it->second;
}




// created a separate file for these string definition 
// functions, so that we can reuse them in the parser
void initializeSetGetCcsHandlerStrings(map<string, string> &typeCodeTable, map<string, string> &classCodeTable){
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

}


