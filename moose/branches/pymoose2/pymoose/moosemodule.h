// moosemodule.h --- 
// 
// Filename: moosemodule.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 17:11:06 2011 (+0530)
// Version: 
// Last-Updated: Fri Mar 25 23:43:35 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 135
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
#include "../basecode/Id.h"
extern "C" {
    typedef struct {
        PyObject_HEAD
        Id _id;
    } _Neutral;
    typedef struct {
        PyObject_HEAD
        ObjId _id;
    } _ObjId;
    typedef struct {
        PyObject_HEAD
        DataId _id;
    } _DataId;
} //!extern "C"


#endif // _MOOSEMODULE_H

// 
// moosemodule.h ends here
