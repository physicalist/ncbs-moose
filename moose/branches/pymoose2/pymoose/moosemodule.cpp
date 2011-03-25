// moosemodule.cpp --- 
// 
// Filename: moosemodule.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 11:26:00 2011 (+0530)
// Version: 
// Last-Updated: Fri Mar 25 17:36:33 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 2432
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
#include <map>

#ifdef USE_MPI
#include <mpi.h>
#endif
#include <iostream>
#include "../basecode/header.h"
#include "../basecode/ObjId.h"
#include "../basecode/DataId.h"
#include "../basecode/ReduceBase.h"
#include "../basecode/ReduceMax.h"
#include "../utility/strutil.h"
#include "../utility/utility.h"
#include "../scheduling/Tick.h"
#include "../scheduling/TickMgr.h"
#include "../scheduling/TickPtr.h"
#include "../scheduling/Clock.h"
#include "../shell/Shell.h"

#include "moosemodule.h"
#include "pymoose.h"
#include "pymoose_Neutral.h"

extern void nonMpiTests(Shell *);
extern void mpiTests();
extern void processTests(Shell *);
extern void regressionTests();
extern unsigned int getNumCores();

using namespace std;
using namespace pymoose;

extern const map<string, string>& pymoose::getArgMap();

static int isSingleThreaded = 0;
static int isInfinite = 0;
static int numNodes = 1;
static int numCores = 1;


void setup_runtime_env(){
    const map<string, string>& argmap = pymoose::getArgMap();
    map<string, string>::const_iterator it;
    it = argmap.find("SINGLETHREADED");
    if (it != argmap.end()){
        istringstream(it->second) >> isSingleThreaded;
    }
    it = argmap.find("INFINITE");
    if (it != argmap.end()){
        istringstream(it->second) >> isInfinite;
    }
    it = argmap.find("NUMCORES");
    if (it != argmap.end()){
        istringstream(it->second) >> numCores;
    }
    it = argmap.find("NUMNODES");
    if (it != argmap.end()){
        istringstream(it->second) >> numNodes;
    }
    cout << "ENVIRONMENT: " << endl
         << "SINGLETHREADED = " << isSingleThreaded << endl
         << "INFINITE = " << isInfinite << endl
         << "NUMCORES = " << numCores << endl
         << "NUMNODES = " << numNodes << endl;
}

// 
// C wrappers for C++ classes
// This is used by Python
extern "C" {
    static PyObject * MooseError;
    static PyObject * SingleThreaded;
    static PyObject * NumNodes;
    static PyObject * NumCores;
    static PyObject * MyNode;
    static PyObject * Infinite;
    
    static PyObject * _pymoose_Neutral_new(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_delete(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_destroy(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_id(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_path(PyObject* dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_setattr(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_getattr(PyObject * dummy, PyObject * args);
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
    
    /**
     * Method definitions.
     */
    static PyMethodDef MooseMethods[] = {
        {"_pymoose_Neutral_new", _pymoose_Neutral_new, METH_VARARGS,
         "Create a new MOOSE element."},
        {"_pymoose_Neutral_delete", _pymoose_Neutral_delete, METH_VARARGS,
         "Destructor for MOOSE element."},
        {"_pymoose_Neutral_id", _pymoose_Neutral_id, METH_VARARGS,
         "return integer representation of the id of the element."},
        {"_pymoose_Neutral_path", _pymoose_Neutral_path, METH_VARARGS,
         "return path of the element."},
        {"_pymoose_Neutral_getattr", _pymoose_Neutral_getattr, METH_VARARGS,
         "get specified attribute of the element."},
        {"_pymoose_Neutral_setattr", _pymoose_Neutral_setattr, METH_VARARGS,
         "set specified attribute of the element."},
        {"_pymoose_Neutral_getFieldNames", _pymoose_Neutral_getFieldNames, METH_VARARGS,
         "get the list field-names."},
        {"_pymoose_Neutral_getChildren", _pymoose_Neutral_getChildren, METH_VARARGS,
         "get list of children"},
        {"_pymoose_Neutral_destroy", _pymoose_Neutral_destroy, METH_VARARGS,
         "destroy the underlying moose element"},
        {"_pymoose_Neutral_connect", _pymoose_Neutral_connect, METH_VARARGS, "Connect another object via a message."},
        {"_pymoose_Neutral_copy", _pymoose_Neutral_copy, METH_VARARGS, "Copy the current element to a target."},
        {"_pymoose_Neutral_move", _pymoose_Neutral_move, METH_VARARGS, "Move the current element"},
        {"_pymoose_Neutral_syncDataHandler", _pymoose_Neutral_syncDataHandler, METH_VARARGS, "?"},
        {"_pymoose_useClock", _pymoose_useClock, METH_VARARGS, "Schedule objects on a specified clock"},
        {"_pymoose_setClock", _pymoose_setClock, METH_VARARGS, "Set the dt of a clock."},
        {"_pymoose_start", _pymoose_start, METH_VARARGS, "Start simulation"},
        {"_pymoose_reinit", _pymoose_reinit, METH_VARARGS, "Reinitialize simulation"},
        {"_pymoose_stop", _pymoose_stop, METH_VARARGS, "Stop simulation"},
        {"_pymoose_isRunning", _pymoose_isRunning, METH_VARARGS, ""},
        {"_pymoose_loadModel", _pymoose_loadModel, METH_VARARGS, ""},
        {"_pymoose_getCwe", _pymoose_getCwe, METH_VARARGS, "Get the current working element."},
        {"_pymoose_setCwe", _pymoose_setCwe, METH_VARARGS, "Set the current working element."},
        {NULL, NULL, 0, NULL}        /* Sentinel */
    };


    /* module initialization */
    PyMODINIT_FUNC init_moose()
    {
        PyObject *moose_module = Py_InitModule("_moose", MooseMethods);
        if (moose_module == NULL)
            return;
        char moose_err[] = "moose.error";
        MooseError = PyErr_NewException(moose_err, NULL, NULL);
        Py_INCREF(MooseError);
        PyModule_AddObject(moose_module, "error", MooseError);
        setup_runtime_env();
        getShell();
        assert (Py_AtExit(&pymoose::finalize) == 0);                
        SingleThreaded = PyInt_FromLong(isSingleThreaded);
        Py_INCREF(SingleThreaded);
        PyModule_AddObject(moose_module, "SINGLETHREADED", SingleThreaded);
        NumCores = PyInt_FromLong(isSingleThreaded);
        Py_INCREF(NumCores);
        PyModule_AddObject(moose_module, "NUMCORES", NumCores);
        NumNodes = PyInt_FromLong(numCores);
        Py_INCREF(NumNodes);
        PyModule_AddObject(moose_module, "NUMNODES", NumNodes);
        MyNode = PyInt_FromLong(numNodes);
        Py_INCREF(MyNode);
        PyModule_AddObject(moose_module, "MYNODE", MyNode);
        Infinite = PyInt_FromLong(isInfinite);
        Py_INCREF(Infinite);
        PyModule_AddObject(moose_module, "INFINITE", Infinite);
        
    }

    // 2011-03-23 15:09:35 (+0530)
    static PyObject * _pymoose_Neutral_new(PyObject * dummy, PyObject * args)
    {
        const char * type;;
        const char * path;
        PyObject * dims = NULL;
        if (!PyArg_ParseTuple(args, "ss|O", &type, &path, &dims))
            return NULL;
        string trimmed_path = path;
        trimmed_path = trim(trimmed_path);
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

        //  paths ending with '/' should raise exception
        if ((length > 1) && (trimmed_path[length - 1] == '/')){
            PyErr_SetString(PyExc_ValueError, "Non-root path must not end with '/'");
            return NULL;
        }
        vector <unsigned int> vec_dims;
        if (dims!= NULL && PySequence_Check(dims)){
            Py_ssize_t len = PySequence_Length(dims);
            for (Py_ssize_t ii = 0; ii < len; ++ ii){
                PyObject* dim = PySequence_GetItem(dims, ii);
                long dim_value = PyInt_AsLong(dim);
                if (dim_value == -1 && PyErr_Occurred()){
                    return NULL;
                }
                vec_dims.push_back((unsigned int)dim_value);
            }                
        }
        if (vec_dims.empty()){
            vec_dims.push_back(1);
        }
        pymoose_Neutral * obj = new pymoose_Neutral(trimmed_path, trimmed_type, vec_dims);
        PyObject * ret = (PyObject *)(obj);
        return ret;
    }

    // 2011-03-23 15:09:13 (+0530)
    static PyObject * _pymoose_Neutral_delete(PyObject * dummy, PyObject * args)
    {
        PyObject * object = NULL;
        if (!PyArg_ParseTuple(args, "O", &object)){
                return NULL;
        }
        pymoose_Neutral * instance = reinterpret_cast<pymoose_Neutral*> (object);
        
        delete instance;
        
        Py_RETURN_NONE;
    }
    // 2011-03-23 15:09:19 (+0530)
    static PyObject* _pymoose_Neutral_id(PyObject * dummy, PyObject * args)
    {
        PyObject * obj = NULL;
        pymoose_Neutral * instance = NULL;
        if (!PyArg_ParseTuple(args, "O", &obj)){
            return NULL;
        }
        instance = reinterpret_cast<pymoose_Neutral*>(obj);
        if (!instance){
            PyErr_SetString(PyExc_TypeError, "Argument cannot be cast to pymoose_Neutral pointer.");
            return NULL;
        }
        unsigned int id = instance->id().value();
        
        PyObject * ret = Py_BuildValue("I", id);
        return ret;
    }
    /**
       Not to be redone. 2011-03-23 14:42:48 (+0530)
     */
    static PyObject * _pymoose_Neutral_path(PyObject * dummy, PyObject * args)
    {
        PyObject * obj = NULL;
        if (!PyArg_ParseTuple(args, "O", &obj)){
            return NULL;
        }
        pymoose_Neutral * instance = reinterpret_cast<pymoose_Neutral*>(obj);
        if (!instance){
            PyErr_SetString(PyExc_TypeError, "Argument cannot be cast to pymoose_Neutral pointer.");
            return NULL;
        }

        string path = instance->id().path();
        PyObject * ret = Py_BuildValue("s", path.c_str());
        return ret;
    }
    /**
       Get a specified field. Redone on: 2011-03-23 14:42:03 (+0530)
     */
    static PyObject * _pymoose_Neutral_getattr(PyObject * dummy, PyObject * args)
    {
        PyObject * obj = NULL;
        const char * field = NULL;
        char ftype;
        unsigned int index = 0;
        if (!PyArg_ParseTuple(args, "Os|i", &obj, &field, &index)){
            return NULL;
        }
        pymoose_Neutral * instance = reinterpret_cast<pymoose_Neutral*>(obj);
        if (!instance){
            PyErr_SetString(PyExc_TypeError, "Argument cannot be cast to pymoose_Neutral pointer.");
            return NULL;
        }
        string field_str(field);
        
        void * ret = instance->getField(field_str, ftype, index);
        
        if (!ret){
            PyErr_SetString(PyExc_RuntimeError, "pymoose_Neutral_getField returned NULL");
            return NULL;
        } else if (!ftype){
            PyErr_SetString(PyExc_RuntimeError, "Invalid field type.");
            return NULL;
        }
        PyObject * pyret = NULL;
        switch(ftype){
            case 'c':
                {
                    pyret = Py_BuildValue("c", *((char*)ret));
                    delete (char*)ret;
                }
                break;
            case 'i':
                {
                    pyret = PyInt_FromLong((long)(*((int*)ret)));                    
                    delete (int*)ret;
                }
                break;
            case 'j':
                {
                    pyret = PyInt_FromLong((long)(*((short*)ret)));                    
                    delete (short*)ret;
                }
            case 'l':
                {
                    pyret = PyLong_FromLong(*((long*)ret));
                    delete (long*)ret;
                }
                break;
            case 'u':
                {
                    pyret = PyLong_FromUnsignedLong((unsigned long)(*((unsigned int*)ret)));
                    delete (unsigned int*)ret;
                }
                break;                
            case 'k': 
                {
                    pyret = PyLong_FromUnsignedLong(*((unsigned long*)ret));
                    delete (unsigned long*)ret;
                }
                break;                
            case 'f':
                {
                    pyret = PyFloat_FromDouble((double)(*((float*)ret)));
                    delete (double*)ret;
                }
                break;
            case 'd':
                {
                    pyret = PyFloat_FromDouble(*((double*)ret));
                    delete (double*)ret;
                }
                break;
            case 's':
                {
                    pyret = PyString_FromString(((string*)ret)->c_str());
                    delete (string*)ret;
                }
                break;
            case 'I':
                {
                                        
                    pyret = (PyObject*)(new pymoose_Neutral(*((Id*)ret)));
                    delete (Id*)ret;
                    
                }
                break;
            case 'O':
                cout << "Getting ObjId field not implemented yet." << endl;
                pyret = Py_None;
            case 'D':
                cout << "Getting DataId field not implemented yet." << endl;
                pyret = Py_None;                
            case 'v':
                {
                    vector <int>& val = *((vector<int>*)ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (unsigned int ii = 0; ii < val.size(); ++ ii ){
                        PyObject * entry = PyInt_FromLong((long)val[ii]);
                        if (!entry || PyTuple_SetItem(pyret, (Py_ssize_t)ii, entry)){
                            free(pyret);
                            pyret = NULL;
                            break;
                        }                        
                    }
                }
                delete (vector<int>*)ret;
                break;
            case 'w':
                {
                    vector<short>& val = *((vector<short> *) ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (unsigned int ii = 0; ii < val.size(); ++ ii){
                        PyObject * entry = PyInt_FromLong((long)val[ii]);                        
                        if (!entry || PyTuple_SetItem(pyret, (Py_ssize_t)ii, entry)){
                            free(pyret);
                            pyret = NULL;
                            break;
                        }
                    }
                }
                delete (vector<short>*)ret;
                break;
            case 'L':
                {
                    vector <long>& val = *((vector<long>*)ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (unsigned int ii = 0; ii < val.size(); ++ ii ){
                        PyObject * entry = PyLong_FromLong(val[ii]);
                        if (!entry || PyTuple_SetItem(pyret, (Py_ssize_t)ii, entry)){
                            free(pyret);
                            pyret = NULL;
                            break;
                        }                        
                    }
                }
                delete (vector<long>*)ret;
                break;
            case 'U':
                {
                    vector <unsigned int>& val = *((vector<unsigned int>*)ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (unsigned int ii = 0; ii < val.size(); ++ ii ){
                        PyObject * entry = PyLong_FromUnsignedLong((unsigned long)val[ii]);
                        if (!entry || PyTuple_SetItem(pyret, (Py_ssize_t)ii, entry)){
                            free(pyret);
                            pyret = NULL;
                            break;
                        }                        
                    }
                }
                delete (vector<unsigned int>*)ret;
                break;
            case 'K':
                {
                    vector <unsigned long>& val = *((vector<unsigned long>*)ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (unsigned int ii = 0; ii < val.size(); ++ ii ){
                        PyObject * entry = PyLong_FromUnsignedLong((unsigned long)val[ii]);
                        if (!entry || PyTuple_SetItem(pyret, (Py_ssize_t)ii, entry)){
                            free(pyret);
                            pyret = NULL;
                            break;
                        }                        
                    }
                }
                delete (vector<unsigned long>*)ret;
                break;
            case 'F':
                {
                    vector <float>& val = *((vector<float>*)ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (unsigned int ii = 0; ii < val.size(); ++ ii ){
                        PyObject * entry = PyFloat_FromDouble((double)val[ii]);
                        if (!entry || PyTuple_SetItem(pyret, (Py_ssize_t)ii, entry)){
                            free(pyret);
                            pyret = NULL;
                            break;
                        }                        
                    }
                }
                delete (vector<float>*)ret;
                break;
            case 'x':
                {
                    vector <double>& val = *((vector<double>*)ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (unsigned int ii = 0; ii < val.size(); ++ ii ){
                        PyObject * entry = PyFloat_FromDouble(val[ii]);
                        if (!entry || PyTuple_SetItem(pyret, (Py_ssize_t)ii, entry)){
                            free(pyret);
                            pyret = NULL;
                            break;
                        }                        
                    }
                }
                delete (vector<double>*)ret;
                break;
            case 'S':
                {
                    vector <string>& val = *((vector<string>*)ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (unsigned int ii = 0; ii < val.size(); ++ ii ){
                        PyObject * entry = PyString_FromString(val[ii].c_str());
                        if (!entry || PyTuple_SetItem(pyret, (Py_ssize_t)ii, entry)){
                            free(pyret);
                            pyret = NULL;
                            break;
                        }                        
                    }
                }
                delete (vector<string>*)ret;
                break;
                
            case 'J':
                {
                    vector<Id>& val = *((vector<Id>*) ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (unsigned int ii = 0; ii < ((vector<Id>*)ret)->size(); ++ ii){
                        
                        pymoose_Neutral * entry = new pymoose_Neutral(val[ii]);
                        
                        if (!entry || PyTuple_SetItem(pyret, (Py_ssize_t)ii, (PyObject*)entry)){
                            free(pyret);
                            pyret = NULL;
                            break;
                        }
                    }                    
                }
                delete (vector<Id>*)ret;
                break;
            case 'P':
                cout << "Getting ObjId field not implemented yet." << endl;
                pyret = Py_None;
            case 'E':
                cout << "Getting DataId field not implemented yet." << endl;
                pyret = Py_None;                
            default:
                {
                    PyErr_SetString(PyExc_TypeError, "Invalid field.");
                    pyret = NULL;
                }
        }
        Py_INCREF(pyret);
        return pyret;        
    }
    /**
       Set a specified field. Redone on 2011-03-23 14:41:45 (+0530)
     */
    static PyObject * _pymoose_Neutral_setattr(PyObject * dummy, PyObject * args)
    {
        
        PyObject * obj = NULL;
        PyObject * value;
        char * field;
        int index = 0;
        if (!PyArg_ParseTuple(args, "OsO|i", &obj, &field,  &value, &index)){
            return NULL;
        }
        pymoose_Neutral * instance = reinterpret_cast<pymoose_Neutral*>(obj);
        if (!instance){
            PyErr_SetString(PyExc_TypeError, "Argument cannot be cast to pymoose_Neutral pointer.");
            return NULL;
        }
        string field_str(field);
        void * value_ptr = NULL;
        char ftype = shortType(instance->getFieldType(field_str));
        
        if (!ftype){
            PyErr_SetString(PyExc_AttributeError, "Field not valid.");
        }
        switch(ftype){
            case 'c':
                {
                    char * pychar = PyString_AsString(value);                    
                    if (pychar){
                        value_ptr = (void*)(new char(pychar[0]));
                    } else {
                        return NULL;
                    }
                    break;
                }
            case 'i':
                {
                    int pyint = PyInt_AsLong(value);
                    if ((pyint == -1) && (PyErr_Occurred())){
                        return NULL;
                    } else {
                        value_ptr = (void*)(new long(pyint));
                    }
                    break;
                }
            case 'j':
                {
                    int pyint = PyInt_AsLong(value);
                    if ((pyint == -1) && (PyErr_Occurred())){
                        return NULL;
                    } else {
                        value_ptr = (void*)(new short(pyint));
                    }
                    break;
                }
            case 'l':
                {
                    long pylong = PyLong_AsLong(value);
                    if ((pylong == -1) && (PyErr_Occurred())){
                        return NULL;
                    } else {
                        value_ptr = (void*)(new long(pylong));
                    }
                    break;
                }
            case 'u':
                {
                    unsigned long pylong = PyLong_AsUnsignedLong(value);
                    value_ptr = (void*)(new long(pylong));
                    break;
                }
            case 'f': 
                {
                    double pydouble = PyFloat_AsDouble(value);                   
                    value_ptr = (void*)(new float(pydouble));
                    break;
                }
            case 'd': 
                {
                    double pydouble = PyFloat_AsDouble(value);                    
                    value_ptr = (void*)(new double(pydouble));
                    break;
                }
            case 's':
                {
                    char * pystr = PyString_AsString(value);
                    if (pystr){
                        value_ptr = (void*)(new string(pystr));
                    } else {
                        return NULL;
                    }
                    break;
                }
            case 'I':
                {
                    
                    Id * id = new Id();
                    pymoose_Neutral* val_obj = reinterpret_cast<pymoose_Neutral*>(value);
                    if (!val_obj){
                        PyErr_SetString(PyExc_TypeError, "Could not cast value to pymoose_Neutral.");
                        return NULL;
                    }
                    * id = val_obj->id();
                    value_ptr = (void*)id;
                    
                }
            case 'O':
                {
                    PyErr_SetString(PyExc_NotImplementedError, "ObjId not yet supported.");
                    return NULL;
                }
            case 'D':
                {
                    PyErr_SetString(PyExc_NotImplementedError, "DataId not yet supported.");
                    return NULL;
                }
            case 'v':
                {
                    if (!PySequence_Check(value)){
                        PyErr_SetString(PyExc_TypeError, "For setting vector<int> field, specified value must be a sequence." );
                        return NULL;
                    }
                    Py_ssize_t length = PySequence_Length(value);
                    vector<int> * vec = new vector<int>();
                    for (unsigned int ii = 0; ii < length; ++ii){
                        int v = PyInt_AsLong(PySequence_GetItem(value, ii));
                        vec->push_back(v);
                    }
                    value_ptr = (void*)vec;
                    break;
                }
            case 'w':
                {
                    if (!PySequence_Check(value)){
                        PyErr_SetString(PyExc_TypeError, "For setting vector<int> field, specified value must be a sequence." );
                        return NULL;
                    }
                    Py_ssize_t length = PySequence_Length(value);
                    vector<short> * vec = new vector<short>();
                    for (unsigned int ii = 0; ii < length; ++ii){
                        short v = PyInt_AsLong(PySequence_GetItem(value, ii));
                        vec->push_back(v);
                    }
                    value_ptr = (void*)vec;
                    break;
                }
            case 'L':
                {
                    if (!PySequence_Check(value)){
                        PyErr_SetString(PyExc_TypeError, "For setting vector<int> field, specified value must be a sequence." );
                        return NULL;
                    }
                    Py_ssize_t length = PySequence_Length(value);
                    vector<long> * vec = new vector<long>();
                    for (unsigned int ii = 0; ii < length; ++ii){
                        long v = PyLong_AsLong(PySequence_GetItem(value, ii));
                        vec->push_back(v);
                    }
                    value_ptr = (void*)vec;
                    break;
                }
            case 'U':
                {
                    if (!PySequence_Check(value)){
                        PyErr_SetString(PyExc_TypeError, "For setting vector<int> field, specified value must be a sequence." );
                        return NULL;
                    }
                    Py_ssize_t length = PySequence_Length(value);
                    vector<unsigned int> * vec = new vector<unsigned int>();
                    for (unsigned int ii = 0; ii < length; ++ii){
                        unsigned long v = PyLong_AsUnsignedLong(PySequence_GetItem(value, ii));
                        vec->push_back((unsigned int)v);
                    }
                    value_ptr = (void*)vec;
                    break;
                }
            case 'K':
                {
                    if (!PySequence_Check(value)){
                        PyErr_SetString(PyExc_TypeError, "For setting vector<int> field, specified value must be a sequence." );
                        return NULL;
                    }
                    Py_ssize_t length = PySequence_Length(value);
                    vector<unsigned long> * vec = new vector<unsigned long>();
                    for (unsigned int ii = 0; ii < length; ++ii){
                        unsigned long v = PyLong_AsUnsignedLong(PySequence_GetItem(value, ii));
                        vec->push_back(v);
                    }
                    value_ptr = (void*)vec;
                    break;
                }
            case 'F':
                {
                    if (!PySequence_Check(value)){
                        PyErr_SetString(PyExc_TypeError, "For setting vector<int> field, specified value must be a sequence." );
                        return NULL;
                    }
                    Py_ssize_t length = PySequence_Length(value);
                    vector<float> * vec = new vector<float>();
                    for (unsigned int ii = 0; ii < length; ++ii){
                        float v = PyFloat_AsDouble(PySequence_GetItem(value, ii));
                        vec->push_back(v);
                    }
                    value_ptr = (void*)vec;
                    break;
                }
            case 'x':
                {
                    if (!PySequence_Check(value)){
                        PyErr_SetString(PyExc_TypeError, "For setting vector<int> field, specified value must be a sequence." );
                        return NULL;
                    }
                    Py_ssize_t length = PySequence_Length(value);
                    vector<double> * vec = new vector<double>();
                    for (unsigned int ii = 0; ii < length; ++ii){
                        double v = PyFloat_AsDouble(PySequence_GetItem(value, ii));
                        vec->push_back(v);
                    }
                    value_ptr = (void*)vec;
                    break;
                }
            case 'S':
                {
                    if (!PySequence_Check(value)){
                        PyErr_SetString(PyExc_TypeError, "For setting vector<string> field, specified value must be a sequence." );
                        return NULL;
                    }
                    Py_ssize_t length = PySequence_Length(value);
                    vector<string> * vec = new vector<string>();
                    for (unsigned int ii = 0; ii < length; ++ii){
                        char * v = PyString_AsString(PySequence_GetItem(value, ii));
                        vec->push_back(string(v));
                    }
                    value_ptr = (void*)vec;
                    break;
                }
            case 'J':
                {
                    if (!PySequence_Check(value)){
                        PyErr_SetString(PyExc_TypeError, "For setting vector<Id> field, specified value must be a sequence." );
                        return NULL;
                    }
                    Py_ssize_t length = PySequence_Length(value);
                    
                    vector<Id> * vec = new vector<Id>();
                    
                    for (unsigned int ii = 0; ii < length; ++ii){
                        pymoose_Neutral* val_obj = reinterpret_cast<pymoose_Neutral*>(PySequence_GetItem(value, ii));
                        if (!val_obj){
                            stringstream msg;
                            msg << "Could not cast value at index " << ii << " to pymoose_Neutral *.";
                            PyErr_SetString(PyExc_TypeError, msg.str().c_str());
                            delete vec;
                            return NULL;
                        }
                        
                        
                        vec->push_back(val_obj->id());
                        
                  }
                    value_ptr = (void*)vec;
                    break;
                }
            case 'P':
                {
                    PyErr_SetString(PyExc_NotImplementedError, "ObjId not yet supported.");
                    return NULL;
                }
            case 'E':
                {
                    PyErr_SetString(PyExc_NotImplementedError, "DataId not yet supported.");
                    return NULL;
                }
                
            default:
                break;
        }
        int ret = instance->setField(field, value_ptr, index);
        
        switch(ftype){
            case 'c': {
                delete (char*)value_ptr;
                break;
            }                
            case 'i': {
                delete (int*) value_ptr;
                break;
            }
            case 'j': {
                delete (short*) value_ptr;
                break;
            }                
            case 'l': {
                delete (long*) value_ptr;
                break;
            }                
            case 'u': {
                delete (unsigned int*) value_ptr;
                break;
            }                
            case 'k': {
                delete (unsigned long*) value_ptr;
                break;
            }                
            case 'f': {
                delete (float*) value_ptr;
                break;
            }                
            case 'd': {
                delete (double*) value_ptr;
                break;
            }                
            case 's': {
                delete (string*) value_ptr;
                break;
            }                
            case 'I': {
                delete (Id*) value_ptr;
                break;
            }                
            case 'O': {
                break;
            }                
            case 'D': {
                break;
            }                
            case 'C': {
                delete (vector <char>*) value_ptr;
                break;
            }                
            case 'v': {
                delete (vector <int>*) value_ptr;
                break;
            }                
            case 'w': {
                delete (vector <short>*) value_ptr;
                break;
            }                
            case 'L': {
                delete (vector <long>*) value_ptr;
                break;
            }                
            case 'U': {
                delete (vector <unsigned int>*) value_ptr;
                break;
            }                
            case 'K': {
                delete (vector <unsigned long>*) value_ptr;
                break;
            }                
            case 'F': {
                delete (vector <float>*) value_ptr;
                break;
            }                
            case 'x': {
                delete (vector <double>*) value_ptr;
                break;
            }                
            case 'S': {
                delete (vector <string>*) value_ptr;
                break;
            }                
            case 'J': {
                delete (vector <Id>*)value_ptr;
                break;
            }                
            case 'P': {
                break;
            }                
            case 'E': {            
                break;
            }         
            default:
                break;
        }
        if (ret){
            Py_RETURN_NONE;
        } else {
            PyErr_SetString(PyExc_TypeError, "The specified field type is not valid.");
            return NULL;
        }
    }
    // 2011-03-23 15:28:26 (+0530)
    static PyObject * _pymoose_Neutral_getFieldNames(PyObject * dummy, PyObject *args)
    {
        PyObject * obj = NULL;
        char * ftype;
        if (!PyArg_ParseTuple(args, "O|s", &obj, &ftype)){
            return NULL;
        }else if ( !ftype || (strlen(ftype) == 0)){
            PyErr_SetString(PyExc_ValueError, "Field type must be a character or string");
            return NULL;
        }
        string ftype_str = string(ftype);
        if ((ftype_str != "srcFinfo") && (ftype_str != "destFinfo") && (ftype_str != "valueFinfo") && (ftype_str != "lookupFinfo") && (ftype_str != "sharedFinfo")){
            PyErr_SetString(PyExc_ValueError, "Invalid finfo type specified. Valid values are: srcFinfo, destFinfo, valueFinfo, lookupFinfo, sharedFinfo");
            return NULL;
        }
        
        pymoose_Neutral * instance = reinterpret_cast<pymoose_Neutral *>(obj);
        if (!instance){
            PyErr_SetString(PyExc_TypeError, "Argument cannot be cast to pymoose_Neutral pointer.");
            return NULL;
        }
        
        vector<string> fieldNames = instance->getFieldNames(ftype_str);
        
        PyObject * pyret = PyTuple_New((Py_ssize_t)fieldNames.size());
        for (unsigned int ii = 0; ii < fieldNames.size(); ++ ii ){
            PyObject * fname = PyString_FromString(fieldNames[ii].c_str());
            if (!pyret){
                pyret = NULL;
                break;
            }
            if (PyTuple_SetItem(pyret, (Py_ssize_t)ii, fname)){
                pyret = NULL;
                break;
            }
        }
        return pyret;
             
    }
    // 2011-03-23 15:13:29 (+0530)
    static PyObject * _pymoose_Neutral_getChildren(PyObject * dummy, PyObject *args)
    {
        PyObject * obj = NULL;
        int index = 0;
        if (!PyArg_ParseTuple(args, "O|i", &obj, &index)){
            return NULL;
        }
        pymoose_Neutral * instance = reinterpret_cast<pymoose_Neutral *>(obj);
        if (!instance){
            PyErr_SetString(PyExc_TypeError, "Argument cannot be cast to pymoose_Neutral pointer.");
            return NULL;
        }
        vector<Id> children = instance->getChildren(index);
        
        PyObject * pyret = PyTuple_New((Py_ssize_t)children.size());
        for (unsigned int ii = 0; ii < children.size(); ++ ii){
            
            pymoose_Neutral * child = new pymoose_Neutral(children[ii]);
            
            if (PyTuple_SetItem(pyret, (Py_ssize_t)ii, (PyObject*)(child))){
                free(pyret);
                pyret = NULL;
                break;
            }
        }
        return pyret;        
    }
    // 2011-03-23 15:14:11 (+0530)
    static PyObject * _pymoose_Neutral_destroy(PyObject * dummy, PyObject * args)
    {
        PyObject * obj = NULL;
        if (!PyArg_ParseTuple(args, "O", &obj)){
            return NULL;
        }
        pymoose_Neutral * instance = reinterpret_cast<pymoose_Neutral *>(obj);
        if (!instance){
            PyErr_SetString(PyExc_TypeError, "Argument cannot be cast to pymoose_Neutral pointer.");
            return NULL;
        }
        instance->destroy();
        
        Py_DECREF(obj);
        Py_RETURN_NONE;
    }
    
    static PyObject * _pymoose_Neutral_connect(PyObject * dummy, PyObject * args)
    {
        PyObject * srcPtr = NULL;
        PyObject * destPtr = NULL;
        unsigned int srcIndex=0, destIndex=0, srcDataIndex=0, destDataIndex=0;
        char * srcField, * destField, * msgType;

        if(!PyArg_ParseTuple(args, "OsOss|iiii", &srcPtr, &srcField, &destPtr, &destField, &msgType, &srcIndex, &destIndex, &srcDataIndex, &destDataIndex)){
            return NULL;
        }
        pymoose_Neutral * src = reinterpret_cast<pymoose_Neutral*>(srcPtr);
        pymoose_Neutral * dest = reinterpret_cast<pymoose_Neutral*>(destPtr);
        bool ret = src->connect(string(srcField), *dest, string(destField), string(msgType), srcIndex, destIndex, srcDataIndex, destDataIndex);
        if (!ret){
            PyErr_SetString(PyExc_NameError, "connect failed: check field names and type compatibility.");
            return NULL;
        }
        return Py_BuildValue("i", ret);
    }
    static PyObject * _pymoose_Neutral_copy(PyObject * dummy, PyObject * args)
    {
        PyErr_SetString(PyExc_NotImplementedError, "Not implemented");
        return NULL;
    }
    static PyObject * _pymoose_Neutral_move(PyObject * dummy, PyObject * args)
    {
        PyErr_SetString(PyExc_NotImplementedError, "Not implemented");
        return NULL;
    }
    static PyObject * _pymoose_Neutral_syncDataHandler(PyObject * dummy, PyObject * args)
    {
        PyErr_SetString(PyExc_NotImplementedError, "Not implemented");
        return NULL;
    }
    // The following are global functions
    static PyObject * _pymoose_useClock(PyObject * dummy, PyObject * args)
    {
        char * path, * field;
        unsigned int tick;
        if(!PyArg_ParseTuple(args, "ssI", &path, &field, &tick)){
            return NULL;
        }
        pymoose::getShell().doUseClock(string(path), string(field), tick);
        Py_RETURN_NONE;
    }
    static PyObject * _pymoose_setClock(PyObject * dummy, PyObject * args)
    {
        unsigned int tick;
        double dt;
        if(!PyArg_ParseTuple(args, "Id", &tick, &dt)){
            return NULL;
        }
        if (dt < 0){
            PyErr_SetString(PyExc_ValueError, "dt must be positive.");
            return NULL;
        }
        pymoose::getShell().doSetClock(tick, dt);
        Py_RETURN_NONE;
    }
    static PyObject * _pymoose_start(PyObject * dummy, PyObject * args)
    {
        double runtime;
        if(!PyArg_ParseTuple(args, "d", &runtime)){
            return NULL;
        }
        if (runtime <= 0.0){
            PyErr_SetString(PyExc_ValueError, "simulation runtime must be positive.");
            return NULL;
        }
        pymoose::getShell().doStart(runtime);
        Py_RETURN_NONE;
    }
    static PyObject * _pymoose_reinit(PyObject * dummy, PyObject * args)
    {
        pymoose::getShell().doReinit();
        Py_RETURN_NONE;
    }
    static PyObject * _pymoose_stop(PyObject * dummy, PyObject * args)
    {
        pymoose::getShell().doStop();
        Py_RETURN_NONE;
    }
    static PyObject * _pymoose_isRunning(PyObject * dummy, PyObject * args)
    {
        return Py_BuildValue("i", pymoose::getShell().isRunning());
    }
    
    static PyObject * _pymoose_loadModel(PyObject * dummy, PyObject * args)
    {
        char * fname, * modelpath;
        if(!PyArg_ParseTuple(args, "ss", &fname, &modelpath)){
            return NULL;
        }
        pymoose_Neutral* model = new pymoose_Neutral(pymoose::getShell().doLoadModel(string(fname), string(modelpath)));
        return reinterpret_cast<PyObject*>(model);
    }

    static PyObject * _pymoose_setCwe(PyObject * dummy, PyObject * args)
    {
        PyObject * element = NULL;
        char * path = "/";
        Id id;
        if(PyArg_ParseTuple(args, "s", &path)){
            id = Id(string(path));
        } else if (!PyArg_ParseTuple(args, "O", &element)){
            return NULL;
        }
        id = (reinterpret_cast<pymoose_Neutral*>(element))->id();
        pymoose::getShell().setCwe(id);
        Py_RETURN_NONE;
    }

    static PyObject * _pymoose_getCwe(PyObject * dummy, PyObject * args)
    {
        Id id = pymoose::getShell().getCwe();
        PyObject * ret = reinterpret_cast<PyObject*>(new pymoose_Neutral(id));
        return ret;
    }

    
} // end extern "C"



//////////////////////////////////////////////
// Main function
//////////////////////////////////////////////


int main(int argc, char* argv[])
{
    for (int ii = 0; ii < argc; ++ii){
        cout << "ARGV: " << argv[ii];
    }
    cout << endl;
    Py_SetProgramName(argv[0]);
    Py_Initialize();
    init_moose();
    return 0;
}

// 
// moosemodule.cpp ends here
