// pymoose.h --- 
// 
// Filename: pymoose.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Fri Mar 11 09:49:33 2011 (+0530)
// Version: 
// Last-Updated: Thu Mar 24 16:17:02 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 164
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
   This class is an attempt to work around module unloading
   limitations in Python 2.X.

   In particular, we want a static singletion instance of this class,
   whose destructor will do the cleanup (like pthreads join and
   MPI_Finalize).
 */
class PyMooseShell
{
  public:
    static PyMooseShell& getInstance();
    Shell& getShell();
  private:
    PyMooseShell();
    ~PyMooseShell();
    Shell * shell_;
    static PyMooseShell * instance_;
};
const std::map<std::string, std::string>& getArgMap();
Shell& getShell();
} // ! namespace pymoose

#endif // !_PYMOOSE_H
// 
// pymoose.h ends here
