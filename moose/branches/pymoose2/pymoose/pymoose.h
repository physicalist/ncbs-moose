// pymoose.h --- 
// 
// Filename: pymoose.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Fri Mar 11 09:49:33 2011 (+0530)
// Version: 
// Last-Updated: Wed Mar 23 10:09:32 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 130
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
    /**
       Creates an instance with a given Id, useful for wrapping
       existing objets.
    */
    pymoose_Neutral(Id id);
    /**
       Create an element with given path and dimensions, type will be
       the class of the element.
     */
    pymoose_Neutral(string path, string type, vector<unsigned int> dims);
    /**
       Destructor does not do anything now.
     */
    ~pymoose_Neutral();
    /**
       Destroy the underlying MOOSE element.
    */
    int destroy();
    /**
       Return a copy of the Id of this object
    */
    Id id();
    /**
       Get the specified field of the index-th object in this element.
       The memory for the field is dynamically allocated and it is the
       responsibility of the caller to invoke the correct deallocation
       method.

       The shorttype of the field is returned in the ftype parameter.
    */
    void * getField(string fname, char& ftype, unsigned int index);
    /**
       Set the specified field's value on object at index.
     */
    int setField(string fname, void * value, unsigned int index);
    /**
       Get a vector containing names of all the fields of a specified kind.

       The kind can be any of:
               sourceFinfo
               destFinfo
               valueFinfo
               lookupFinfo
               sharedFinfo
    */
    vector<string> getFieldNames(string ftypeType);
    /**
       Returns the type of the specified field on this element.
     */
    string getFieldType(string field);
    /**
       Returns a list of Ids of all the children of the object at
       specified index.
    */
    vector<Id> getChildren(unsigned int index);    
  protected:
    /**
       id_ is the sole reference to MOOSE elements.
    */
    Id id_;
};

/**
   Returns a global static map created using environment
   variables/command line arguments.  Equivalent to Property utility
   in earlier moose.  This will be used by getShell to access global
   parameters.

   Currently, it uses environment variables to populate the map. The
   following environment values are recognized:

   NUMNODES -- number of nodes in case of parallel simulation.

   NUMCORES -- number of cores on this node

   SINGLETHREADED -- 1 if this MOOSE should run in a single thread, 0
   if it is to run mutithreaded mode.

   INFINITE -- this is for testing.
   

   
*/
const map<string, string>* getArgMap();
/**
   Initialized and returns a reference to the Shell.
*/
const Shell& getShell();

} // ! namespace pymoose

#endif // !_PYMOOSE_H
// 
// pymoose.h ends here
