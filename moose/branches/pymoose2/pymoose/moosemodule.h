// moosemodule.h --- 
// 
// Filename: moosemodule.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 17:11:06 2011 (+0530)
// Version: 
// Last-Updated: Mon Mar 14 22:49:04 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 107
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
#ifndef _MOOSEMODULE_H
#define _MOOSEMODULE_H

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
    virtual ~pymoose_Neutral();
    std::string id_str();
  private:
    const Id* id_;
};

} // namespace pymoose

// 
// moosemodule.h ends here
#endif // _MOOSEMODULE_H
