// moosemodule.h --- 
// 
// Filename: moosemodule.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 17:11:06 2011 (+0530)
// Version: 
// Last-Updated: Thu Mar 10 17:45:17 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 28
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

#include "shell/Shell.h"
#include "basecode/Id.h"
#include "Python.h"
namespace pymoose{
static Shell* __shell;
extern "C" {
    int setClock(unsigned int tickNum, double dt);
    int setCwe(PyObject * self, PyObject * Id_cwe); // Id_cwe will stand for Id pointer
    PyObject* getCwe(PyObject *self);
    PyObject* doCreate(PyObject* self, const char* type, PyObject * Id_parent, const char * name, PyObject* vuint_dimensions);
    int doDelete( PyObject* self, PyObject* Id_target);
    
}
}


// 
// moosemodule.h ends here
