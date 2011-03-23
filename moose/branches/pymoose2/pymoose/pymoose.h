// pymoose.h --- 
// 
// Filename: pymoose.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Fri Mar 11 09:49:33 2011 (+0530)
// Version: 
// Last-Updated: Tue Mar 22 17:01:34 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 94
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// 
// 
// 

// Change log:
// 
// 2011-03-11 09:50:06 (+0530) Dividing C++ wrapper and C externals
// for pymoose.
// 

// Code:
#ifndef _PYMOOSE_H
#define _PYMOOSE_H
#include "Python.h"
#include <string>
class Shell;
class Id;

namespace pymoose{
/**
 * Base of the whole PyMoose class hierarchy.
 */
class PyMooseBase
{
  public:
    PyMooseBase();
    virtual ~PyMooseBase();    
}; // ! class PyMooseBase

/**
 * Wraps Neutral.
 */
class pymoose_Neutral: public PyMooseBase
{
  public:
    pymoose_Neutral(Id id);
    pymoose_Neutral(string path, string type, vector<unsigned int> dims);
    virtual ~pymoose_Neutral();
    int destroy();
    Id id();
    void * getField(unsigned int index, string fname, char ftype);
    int setField(unsigned int index, string fname, char ftype, void * value);
    vector<string> getFieldNames(char ftypeType);
    string getFieldType(string field);
    vector<Id> getChildren(unsigned int index=0);    
  protected:
    Id id_;
};

/// Returns a global static map created using environment variables/command line arguments.
/// Equivalent to Property utility in earlier moose.
/// This will be used by getShell to access global parameters
const map<string, string>* getArgMap();
const Shell& getShell();

} // ! namespace pymoose

#endif // !_PYMOOSE_H
// 
// pymoose.h ends here
