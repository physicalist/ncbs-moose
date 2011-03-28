// moosemodule.h --- 
// 
// Filename: moosemodule.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 17:11:06 2011 (+0530)
// Version: 
// Last-Updated: Tue Mar 29 00:15:05 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 236
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
    /**
       _Neutral wraps the Neutral class - where each element is identified by Id
    */
    typedef struct {
        PyObject_HEAD
        Id _id;
    } _Neutral;
    /**
       _Element wraps the subelements of a Neutral - identified by
       index. This is different from the Element in GENESIS
       terminology. Since Neutral is now by default an array element,
       we call the individual entries in it as Elements.

       According to MOOSE API, ObjId is a composition of Id and DataId
       - thus uniquely identifying a field of a subelement of Neutral.

       Since the individual subelements are identified by their ObjId
       only (there is no intermediate id for the subelements except
       ObjId with DataId(index, 0) ), we use ObjId for recognizing
       both.
    */
    typedef struct {
        PyObject_HEAD
        ObjId _id;
    } _Element;

    static PyObject * MooseError;
    //////////////////////////////////////////
    // Methods for Neutral class
    //////////////////////////////////////////
    static int _pymoose_Neutral_init(_Neutral * self, PyObject * args, PyObject * kwargs);
    static void _pymoose_Neutral_dealloc(_Neutral * self);
    static PyObject * _pymoose_Neutral_repr(_Neutral * self);
    static PyObject * _pymoose_Neutral_str(_Neutral * self);
    static PyObject * _pymoose_Neutral_destroy(_Neutral * self, PyObject * args);
    static PyObject * _pymoose_Neutral_getId(_Neutral * self, PyObject * args);
    static PyObject * _pymoose_Neutral_getPath(_Neutral * self, PyObject * args);
    static PyObject * _pymoose_Neutral_syncDataHandler(_Neutral * self, PyObject * args);
    /* Neutral functions to allow part of sequence protocol */
    static Py_ssize_t _pymoose_Neutral_getLength(_Neutral * self);
    static PyObject * _pymoose_Neutral_getItem(_Neutral * self, Py_ssize_t index);
    static PyObject * _pymoose_Neutral_getSlice(_Neutral * self, PyObject * args);    
    static PyObject * _pymoose_Neutral_getShape(_Neutral * self, PyObject * args);    
    static int _pymoose_Neutral_richCompare(_Neutral * self, PyObject * args, int op);
    static int _pymoose_Neutral_contains(_Neutral * self, PyObject * args);
    ///////////////////////////////////////////
    // Methods for Element class
    ///////////////////////////////////////////
    static int _pymoose_Element_init(_Element * self, PyObject * args, PyObject * kwargs);
    static void _pymoose_Element_dealloc(_Element * self);
    static PyObject * _pymoose_Element_repr(_Element * self);
    static PyObject * _pymoose_Element_str(_Element * self);
    static PyObject * _pymoose_Element_getField(_Element * self, PyObject * args);
    static PyObject * _pymoose_Element_setField(_Element * self, PyObject * args);
    static PyObject * _pymoose_Element_getFieldNames(_Element * self, PyObject * args);
    static PyObject * _pymoose_Element_getFieldType(_Element * self, PyObject * args);
    static PyObject * _pymoose_Element_getDataIndex(_Element * self, PyObject * args);
    static PyObject * _pymoose_Element_getFieldIndex(_Element * self, PyObject * args);
    static PyObject * _pymoose_Element_getId(_Element * self, PyObject * args);
    static PyObject * _pymoose_Element_connect(_Element * self, PyObject * args);
    static int _pymoose_Element_richCompare(_Element * self, PyObject * args, int op);
    
    ////////////////////////////////////////////////
    // static functions to be accessed from Python
    ////////////////////////////////////////////////


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
    static PyObject * _pymoose_copy(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_move(PyObject * dummy, PyObject * args);

    PyMODINIT_FUNC init_moose();
} //!extern "C"


#endif // _MOOSEMODULE_H

// 
// moosemodule.h ends here
