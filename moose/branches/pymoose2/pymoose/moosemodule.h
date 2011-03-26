// moosemodule.h --- 
// 
// Filename: moosemodule.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 17:11:06 2011 (+0530)
// Version: 
// Last-Updated: Sat Mar 26 19:16:31 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 152
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
        ObjId _id;
    } _Neutral;

    static PyObject * MooseError;
    //////////////////////////////////////////
    // Methods for Neutral class
    //////////////////////////////////////////
    static void _pymoose_Neutral_dealloc(_Neutral * self);
    static PyObject * _pymoose_Neutral_repr(_Neutral * self);
    static int _pymoose_Neutral_init(_Neutral * self, PyObject * args, PyObject * kwargs);
    static PyObject * _pymoose_Neutral_destroy(_Neutral * self, PyObject * args);
    static PyObject * _pymoose_Neutral_getField(_Neutral * self, PyObject * args);
    static PyObject * _pymoose_Neutral_getFieldType(_Neutral * self, PyObject * args);
    ////////////////////////////////////////////////
    // static functions to be accessed from Python
    ////////////////////////////////////////////////
    static PyObject * _pymoose_Neutral_new(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_delete(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_id(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_path(PyObject* dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_setattr(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_getFieldNames(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_getChildren(PyObject * dummy, PyObject *args);
    static PyObject * _pymoose_Neutral_connect(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_copy(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_move(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_syncDataHandler(PyObject * dummy, PyObject * args);
    // The following are global functions
    static PyObject * _pymoose_useClock(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_setClock(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_start(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_reinit(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_stop(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_isRunning(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_loadModel(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_setCwe(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_getCwe(PyObject * dummy, PyObject * args);

    PyMODINIT_FUNC init_moose();
} //!extern "C"


#endif // _MOOSEMODULE_H

// 
// moosemodule.h ends here
