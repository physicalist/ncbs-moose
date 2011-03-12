// moosemodule.h --- 
// 
// Filename: moosemodule.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 17:11:06 2011 (+0530)
// Version: 
// Last-Updated: Sat Mar 12 23:58:10 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 97
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
// 
// 

// Code:


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
class _pymoose_Neutral: public PyMooseBase
{
  public:
    _pymoose_Neutral(Id id);
    ~_pymoose_Neutral();
    
  private:
    const Id* id_;
};

} // namespace pymoose

// 
// moosemodule.h ends here
