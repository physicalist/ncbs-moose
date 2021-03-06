// types.cpp --- 
// 
// Filename: types.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Wed Mar 23 10:10:45 2011 (+0530)
// Version: 
// Last-Updated: Tue Jun 19 16:52:07 2012 (+0530)
//           By: subha
//     Update #: 61
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// Some utility functions to give short character representations for
// particular type names.
// 
// 

// Change log:
// 
// 
// 

// Code:
#include <string>
#include <map>

using namespace std;

/**
   returns a single character code for a given string representation of data type.
   Mostly follows Python/C API's PyArg_ParseTuple convention.
*/
char shortType(string name)
{
    static map<string, char> typemap;
    if (typemap.empty()){
        typemap.insert(pair<string, char>("bool", 'b'));
        typemap.insert(pair<string, char>("char", 'c'));
        typemap.insert(pair<string, char>("int", 'i'));
        typemap.insert(pair<string, char>("short", 'h'));
        typemap.insert(pair<string, char>("long", 'l'));
        typemap.insert(pair<string, char>("unsigned int", 'I'));
        typemap.insert(pair<string, char>("unsigned long", 'k'));        
        typemap.insert(pair<string, char>("float", 'f'));
        typemap.insert(pair<string, char>("double", 'd'));
        typemap.insert(pair<string, char>("string", 's'));
        typemap.insert(pair<string, char>("Id", 'x'));
        typemap.insert(pair<string, char>("ObjId", 'y'));
        typemap.insert(pair<string, char>("DataId", 'z'));
        typemap.insert(pair<string, char>("vector<char>", 'C'));
        typemap.insert(pair<string, char>("vector<int>", 'v'));
        typemap.insert(pair<string, char>("vector<short>", 'w'));
        typemap.insert(pair<string, char>("vector<long>", 'L'));
        typemap.insert(pair<string, char>("vector<unsigned int>", 'U'));
        typemap.insert(pair<string, char>("vector<unsigned long>", 'K'));        
        typemap.insert(pair<string, char>("vector<float>", 'F'));
        typemap.insert(pair<string, char>("vector<double>", 'D'));
        typemap.insert(pair<string, char>("vector<string>", 'S'));
        typemap.insert(pair<string, char>("vector<Id>", 'X'));
        typemap.insert(pair<string, char>("vector<ObjId>", 'Y'));
        typemap.insert(pair<string, char>("vector<DataId", 'Z'));
        typemap.insert(pair<string, char>("void", '_'));
        typemap.insert(pair<string, char>("vector< vector<unsigned int> >", 'P'));
        typemap.insert(pair<string, char>("vector< vector<int> >", 'Q'));
        typemap.insert(pair<string, char>("vector< vector<double> >", 'R'));
    }
    map<string, char>::iterator iter = typemap.find(name);
    if (iter == typemap.end()){
        return 0;
    }
    return iter->second;
}

char shortFinfo(string finfoType)
{
    static map<string, char> finfomap;
    if (finfomap.empty()){
        finfomap.insert(pair<string, char>("srcFinfo", 's'));
        finfomap.insert(pair<string, char>("destFinfo", 'd'));
        finfomap.insert(pair<string, char>("sharedFinfo", 'x'));
        finfomap.insert(pair<string, char>("valueFinfo", 'v'));
        finfomap.insert(pair<string, char>("lookupFinfo", 'l'));
    }
    map <string, char>::iterator iter = finfomap.find(finfoType);
    if (iter == finfomap.end()){
        return 0;
    }
    return iter->second;
}

// 
// types.cpp ends here
