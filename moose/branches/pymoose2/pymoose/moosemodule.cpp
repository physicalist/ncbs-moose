// moosemodule.cpp --- 
// 
// Filename: moosemodule.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 11:26:00 2011 (+0530)
// Version: 
// Last-Updated: Sat Mar 12 23:54:15 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 448
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

#include "moosemodule.h"
#include "header.h"
#include "ReduceBase.h"
#include "ReduceMax.h"
#include "Shell.h"
#include "../utility/strutil.h"

using namespace std;
using namespace pymoose;
PyMooseBase::PyMooseBase()
{
}
PyMooseBase::~PyMooseBase()
{
}

PyMooseNeutral::PyMooseNeutral(Id id)
{
    this->id_ = new Id(id);
}
PyMooseNeutral::~PyMooseNeutral()
{
    delete this->id_;
}

Shell* getShell()
{
    static Shell* shell = NULL;
    if (shell == NULL)
        shell = new Shell();
    return shell;
}

extern "C" {
    static PyObject * MooseError;    
    static PyObject * __shell;
    static PyObject * moose_test_dummy(PyObject* dummy, PyObject* args);
    // static PyObject * _pymoose_Neutral_new(PyObject * dummy, PyObject * args);
    static PyObject * _PyMooseNeutral_new(PyObject * dummy, PyObject * args);
    /**
     * Method definitions.
     */
    static PyMethodDef MooseMethods[] = {
        {"test_dummy",  moose_test_dummy, METH_VARARGS,
         "A test function."},
        // {"_pymoose_Neutral_new", _pymoose_Neutral_new, METH_VARARGS,
        //  "Create a new MOOSE element."},
        {"_PyMooseNeutral_new", _PyMooseNeutral_new, METH_VARARGS,
         "Create a new MOOSE element."},
        {NULL, NULL, 0, NULL}        /* Sentinel */
    };

    static PyObject *  moose_test_dummy(PyObject* dummy, PyObject* args)
    {
        const char * param;
        if (!PyArg_ParseTuple(args, "s", &param))
            return NULL;
        
        return PyString_InternFromString(param);
    }

    /* module initialization */
    PyMODINIT_FUNC init_moose(void)
    {
        PyObject *moose_module = Py_InitModule("_moose", MooseMethods);
        if (moose_module == NULL)
            return;
        MooseError = PyErr_NewException("moose.error", NULL, NULL);
        Py_INCREF(MooseError);
        PyModule_AddObject(moose_module, "error", MooseError);
        __shell = (PyObject *)(getShell());
        Py_INCREF(__shell);
    }
    
    static PyMooseNeutral * PyMooseNeutral_new(PyObject * dummy, PyObject * args)
    {
        const char * type;
        const char * path;
        PyObject * dims;
        if (!PyArg_ParseTuple(args, "ssO", &type, &path, &dims))
            return NULL;
        string str_path = string(path);
        str_path = trim(str_path);
        size_t length = str_path.length();
        if (length <= 0){
            return NULL;
        }
        Id id;
        if (length > 1){
            if (str_path[length - 1] == '/'){
                str_path = str_path.substr(0, length-1);
            }
            id = Id(str_path);

            if (id == Id()) { // object does not exist
                size_t pos = str_path.rfind('/');
                string parent_path = str_path.substr(0, pos);
                Id parent_id = Id(parent_path);
                string name = str_path.substr(pos);

                vector<unsigned int> vec_dims;                
                if (PySequence_Check(dims)){
                    Py_ssize_t len = PySequence_Length(dims);
                    for (Py_ssize_t ii = 0; ii < len; ++ ii){
                        PyObject* dim = PySequence_GetItem(dims, ii);
                        unsigned int dim_value;
                        if (PyArg_ParseTuple(dim, "I", &dim_value)){
                            vec_dims.push_back(dim_value);
                        }
                    }
                }
                id = getShell()->doCreate(string("Neutral"), Id(parent_path), string(name), vector<unsigned int>(vec_dims));
            }
        }
        return new PyMooseNeutral(id);
    }

    static PyObject* _PyMooseNeutral_new(PyObject* dummy, PyObject* args)
    {
        PyObject* obj = (PyObject*)PyMooseNeutral_new(dummy, args);
        return obj;
    }
#if 0
    static PyObject* _pymoose_Neutral_new(PyObject * dummy, PyObject * args)
    {
        const char * type;;
        const char * path;
        PyObject * dims = NULL;
        if (!PyArg_ParseTuple(args, "ss|O", &type, &path, &dims))
            return NULL;
        string trimmed_path = trim(string(path));
        size_t length = trimmed_path.length();
        if (length <= 0){
            PyErr_SetString(PyExc_ValueError, "path must be non-empty string.");
            return NULL;
        }
        string trimmed_type = trim(string(type));
        if (trimmed_type.length() <= 0){
            PyErr_SetString(PyExc_ValueError, "type must be non-empty string.");
            return NULL;
        }        
        
        if ((length > 1) && (trimmed_path[length - 1] == '/')){
            trimmed_path = trimmed_path.substr(0, length-1);
        }
        if (trimmed_path[0] != '/'){ // Convert relative path to absolute path
            Id cwe = getShell()->getCwe();
            trimmed_path = cwe.path() + trimmed_path;
            length = trimmed_path.length();
        }
        Id id = Id(trimmed_path);
        if (length > 1 && id == Id()) { // object does not exist
            size_t pos = trimmed_path.rfind('/');
            string parent_path = trimmed_path.substr(0, pos);
            Id parent_id = Id(parent_path);
            string name = trimmed_path.substr(pos);
            vector <unsigned int> vec_dims;
            if (dims!= NULL && PySequence_Check(dims)){
                Py_ssize_t len = PySequence_Length(dims);
                for (Py_ssize_t ii = 0; ii < len; ++ ii){
                    PyObject* dim = PySequence_GetItem(dims, ii);
                    unsigned int dim_value;
                    if (PyArg_ParseTuple(dim, "I", &dim_value)){
                        vec_dims.push_back(dim_value);
                    }
                }
            }
            if (vec_dims.empty()){
                vec_dims.push_back(1);
            }
            id = getShell()->doCreate(trimmed_type, Id(parent_path), name, vec_dims);
        }
        return (PyObject*)(new _pymoose_Neutral(id));
    }
#endif

} // end extern "C"


int main(int argc, char* argv[])
{
    Py_SetProgramName(argv[0]);
    Py_Initialize();
    init_moose();
    return 0;
}

// 
// moosemodule.cpp ends here
