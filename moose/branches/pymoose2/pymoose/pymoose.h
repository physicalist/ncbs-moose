// pymoose.h --- 
// 
// Filename: pymoose.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Fri Mar 11 09:49:33 2011 (+0530)
// Version: 
// Last-Updated: Wed Mar 23 11:42:25 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 141
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
#include <string>
#include <map>

class Shell;
class Id;
namespace pymoose{
using namespace std;
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
const map<string, string>& getArgMap();
/**
   Initialized and returns a reference to the Shell.
*/
const Shell& getShell();

} // ! namespace pymoose

#endif // !_PYMOOSE_H
// 
// pymoose.h ends here
