// moosemodule.h --- 
// 
// Filename: moosemodule.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 17:11:06 2011 (+0530)
// Version: 
// Last-Updated: Sun Mar 13 01:13:05 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 98
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
class pymoose_Neutral: public PyMooseBase
{
  public:
    pymoose_Neutral(Id id);
    ~pymoose_Neutral();
    
  private:
    const Id* id_;
};

} // namespace pymoose

// 
// moosemodule.h ends here
