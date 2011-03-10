// moosemodule.cpp --- 
// 
// Filename: moosemodule.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 11:26:00 2011 (+0530)
// Version: 
// Last-Updated: Thu Mar 10 13:51:34 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 48
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// 2011-03-10 Trying out direct access to Python API instead of going
//            via SWIG. SWIG has this issue of creating huge files and
//            the resulting binaries are also very large. Since we are
//            not going to use any language but Python in the
//            foreseeable future, we could as well try to avoid the
//            bloat by coding directly with Python API.
//

// TODO:
//
// Port the low level API.
//
// May use the low level API in Python to create the high level API.
//
// Allow exceptions in MOOSE. 

// Change log:
// 
// 2011-03-10 Initial version. Starting coding directly with Python API.
// 

// Code:

#include <Python.h>

extern "C" {
    static PyObject *MooseError;

    static PyObject *  moose_test(PyObject* self, PyObject* args);
    
    static PyMethodDef MooseMethods[] = {
    {"test",  moose_test, METH_VARARGS,
     "A test function."},    
    {NULL, NULL, 0, NULL}        /* Sentinel */
    };

    static PyObject *  moose_test(PyObject* self, PyObject* args)
    {
        const char * param;
        if (!PyArg_ParseTuple(args, "S", param))
            return NULL;
        
        return PyString_InternFromString(param);
    }
    PyMODINIT_FUNC
    initmoose(void)
    {
        PyObject *moose_module = Py_InitModule("moose", MooseMethods);
        if (moose_module == NULL)
            return;
        MooseError = PyErr_NewException("moose.error", NULL, NULL);
        Py_INCREF(MooseError);
        PyModule_AddObject(moose_module, "error", MooseError);        
    }

}
    int main(int argc, char* argv[])
    {
        Py_SetProgramName(argv[0]);
        Py_Initialize();
        initmoose();
        return 0;
    }

// 
// moosemodule.cpp ends here
