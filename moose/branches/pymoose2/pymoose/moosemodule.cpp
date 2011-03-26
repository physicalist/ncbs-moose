// moosemodule.cpp --- 
// 
// Filename: moosemodule.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 11:26:00 2011 (+0530)
// Version: 
// Last-Updated: Sat Mar 26 19:19:30 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 2781
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
static int myNode = 0;

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
string getFieldType(ObjId id, string fieldName)
{
    string fieldType;
    string className = Field<string>::get(id, "class");
    string classInfoPath("/classes/" + className);
    Id classId(classInfoPath);
    assert(classId != Id());
    static vector<string> finfotypes;
    if (finfotypes.empty()){
        finfotypes.push_back("srcFinfo");
        finfotypes.push_back("destFinfo");
        finfotypes.push_back("valueFinfo");
        finfotypes.push_back("lookupFinfo");
        finfotypes.push_back("sharedFinfo");        
    }

    for (unsigned jj = 0; jj < finfotypes.size(); ++ jj){
        unsigned int numFinfos = Field<unsigned int>::get(ObjId(classId, 0), "num_" + finfotypes[jj]);
        Id fieldId(classId.path() + "/" + finfotypes[jj]);
        for (unsigned int ii = 0; ii < numFinfos; ++ii){
            string _fieldName = Field<string>::get(ObjId(fieldId, DataId(0, ii)), "name");
            if (fieldName == _fieldName){
                fieldType = Field<string>::get(ObjId(fieldId, DataId(0, ii)), "type");
                return fieldType;
            }
        }
    }
    return fieldType;        
}

// 
// C wrappers for C++ classes
// This is used by Python
extern "C" {
    static PyMethodDef NeutralMethods[] = {
        {"getField", (PyCFunction)_pymoose_Neutral_getField, METH_VARARGS,
         "get specified attribute of the element."},
        {"_pymoose_Neutral_destroy", (PyCFunction)_pymoose_Neutral_destroy, METH_VARARGS,
         "destroy the underlying moose element"},
        {NULL, NULL, 0, NULL},        /* Sentinel */        
    };
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
        {"_pymoose_Neutral_setattr", _pymoose_Neutral_setattr, METH_VARARGS,
         "set specified attribute of the element."},
        {"_pymoose_Neutral_getFieldNames", _pymoose_Neutral_getFieldNames, METH_VARARGS,
         "get the list field-names."},
        {"_pymoose_Neutral_getChildren", _pymoose_Neutral_getChildren, METH_VARARGS,
         "get list of children"},
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

    ///////////////////////////////////////////////
    // Type defs for PyObject of Neutral
    ///////////////////////////////////////////////
    static PyTypeObject NeutralType = { 
        PyObject_HEAD_INIT(0)               /* tp_head */
        0,                                  /* tp_internal */
        "moose.Neutral",                  /* tp_name */
        sizeof(_Neutral),                    /* tp_basicsize */
        0,                                  /* tp_itemsize */
        (destructor)_pymoose_Neutral_dealloc,                    /* tp_dealloc */
        0,                                  /* tp_print */
        0,                                  /* tp_getattr */
        0,                                  /* tp_setattr */
        0,                                  /* tp_compare */
        (reprfunc)_pymoose_Neutral_repr,                        /* tp_repr */
        0,                                  /* tp_as_number */
        0,                                  /* tp_as_sequence */
        0,                                  /* tp_as_mapping */
        0,                                  /* tp_hash */
        0,                                  /* tp_call */
        0,                                  /* tp_str */
        PyObject_GenericGetAttr,            /* tp_getattro */
        PyObject_GenericSetAttr,            /* tp_setattro */
        0,                                  /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,
        "",
        0,                                  /* tp_traverse */
        0,                                  /* tp_clear */
        0,                                  /* tp_richcompare */
        0,                                  /* tp_weaklistoffset */
        0,                                  /* tp_iter */
        0,                                  /* tp_iternext */
        NeutralMethods,                     /* tp_methods */
        0,                    /* tp_members */
        0,                                  /* tp_getset */
        0,                                  /* tp_base */
        0,                                  /* tp_dict */
        0,                                  /* tp_descr_get */
        0,                                  /* tp_descr_set */
        0,                                  /* tp_dictoffset */
        (initproc) _pymoose_Neutral_init,                       /* tp_init */
        PyType_GenericAlloc,                /* tp_alloc */
        PyType_GenericNew,                  /* tp_new */
        _PyObject_Del,                      /* tp_free */
    };

#define Neutral_Check(v) (Py_TYPE(v) == &NeutralType)

    /* module initialization */
    PyMODINIT_FUNC init_moose()
    {
        PyObject *moose_module = Py_InitModule3("_moose", MooseMethods, "MOOSE = Multiscale Object-Oriented Simulation Environment.");
        if (moose_module == NULL)
            return;
        char moose_err[] = "moose.error";
        MooseError = PyErr_NewException(moose_err, NULL, NULL);
        Py_INCREF(MooseError);
        PyModule_AddObject(moose_module, "error", MooseError);
        NeutralType.tp_new = PyType_GenericNew;
        NeutralType.tp_free = _PyObject_Del;
        if (PyType_Ready(&NeutralType) < 0)
            return;
        Py_INCREF(&NeutralType);
        PyModule_AddObject(moose_module, "Neutral", (PyObject*)&NeutralType);
        
        setup_runtime_env();
        getShell();
        assert (Py_AtExit(&pymoose::finalize) == 0);                
        PyModule_AddIntConstant(moose_module, "SINGLETHREADED", isSingleThreaded);
        PyModule_AddIntConstant(moose_module, "NUMCORES", numCores);
        PyModule_AddIntConstant(moose_module, "NUMNODES", numNodes);
        PyModule_AddIntConstant(moose_module, "MYNODE", myNode);
        PyModule_AddIntConstant(moose_module, "INFINITE", isInfinite);
        
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
       Get a specified field. Re-done on: 2011-03-23 14:42:03 (+0530)

       I wonder how to cleanly do this. The Id - ObjId dichotomy is
       really ugly. When you don't pass an index, it is just treated
       as 0. Then what is the point of having Id separately? ObjId
       would been just fine!
     */
    static PyObject * _pymoose_Neutral_getField(_Neutral * self, PyObject * args)
    {
        const char * field = NULL;
        char ftype;
        if (!PyArg_ParseTuple(args, "s", &field)){
            return NULL;
        }
        PyObject * ret;
        string field_str(field);
        // The GET_FIELD macro is just a short-cut to reduce typing
        // TYPE is the full type string for the field. TYPEC is the corresponding Python Py_BuildValue format character.
#define GET_FIELD(TYPE, TYPEC)                                          \
        {ret = Py_BuildValue(#TYPEC, Field<TYPE>::get(self->_id, string(field))); break;}
        
#define GET_VECFIELD(TYPE, TYPEC) \
        {                                                               \
                vector<TYPE> val = Field< vector<TYPE> >::get(self->_id, string(field)); \
                ret = PyTuple_New((Py_ssize_t)val.size());              \
                for (unsigned int ii = 0; ii < val.size(); ++ ii ){     \
                        PyObject * entry = Py_BuildValue(#TYPEC, val[ii]); \
                        if (!entry || PyTuple_SetItem(ret, (Py_ssize_t)ii, entry)){ \
                            free(ret);                                  \
                            ret = NULL;                                 \
                            break;                                      \
                        }                                               \
                }                                                       \
                break;                                                  \
        }
        
        string type = getFieldType(self->_id, string(field));
        ftype = shortType(type);
        switch(ftype){
            case 'c': GET_FIELD(char, c)
            case 'i': GET_FIELD(int, i)
            case 'h': GET_FIELD(short, h)
            case 'l': GET_FIELD(long, l)        
            case 'I': GET_FIELD(unsigned int, I)
            case 'k': GET_FIELD(unsigned long, k)
            case 'f': GET_FIELD(float, f)
            case 'd': GET_FIELD(double, d)
            case 's': {
                string _s = Field<string>::get(self->_id, string(field));
                ret = Py_BuildValue("s", _s.c_str());
                break;
            }
                    // case 'I': {
                    //     GET_FIELD(Id)
                    // }
                    // case 'O': {
                    //     GET_FIELD(ObjId)
                    // }
                    // case 'D': {
                    //     GET_FIELD(DataId)
                    // }
            case 'v': GET_VECFIELD(int, i)
            case 'w': GET_VECFIELD(short, h)
            case 'L': GET_VECFIELD(long, l)        
            case 'U': GET_VECFIELD(unsigned int, I)        
            case 'K': GET_VECFIELD(unsigned long, k)        
            case 'F': GET_VECFIELD(float, f)        
            case 'D': GET_VECFIELD(double, d)        
            case 'S': {                                                 
                vector<string> val = Field< vector<string> >::get(self->_id, string(field)); 
                ret = PyTuple_New((Py_ssize_t)val.size());              
                for (unsigned int ii = 0; ii < val.size(); ++ ii ){     
                    PyObject * entry = Py_BuildValue("s", val[ii].c_str()); 
                    if (!entry || PyTuple_SetItem(ret, (Py_ssize_t)ii, entry)){ 
                        free(ret);                                  
                        ret = NULL;                                 
                        break;                                      
                    }                                               
                }                                                       
                break;                                                  
            }
            default:
                PyErr_SetString(PyExc_TypeError, "Unrecognized field type.");
                ret = NULL;            
        }
#undef GET_FIELD    
#undef GET_VECFIELD        
        return ret;        
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

    ///////////////////////////////////////////////////////////////////
    // New set of functions for native Python data type
    ///////////////////////////////////////////////////////////////////
    static PyObject * _pymoose_Neutral_repr(_Neutral * self)
    {
        return PyString_FromFormat("<Neutral: Id=%u, Data:=%u, Field=%u>", self->_id.id.value(), self->_id.dataId.data(), self->_id.dataId.field());
    }
    
    static void _pymoose_Neutral_dealloc(_Neutral * self)
    {
        PyObject_Del(self);
    }

    // 2011-03-23 15:14:11 (+0530)
    // 2011-03-26 17:02:19 (+0530)
    //
    // 2011-03-26 19:14:34 (+0530) - This IS UGLY! Destroying one
    // ObjId will destroy the containing element and invalidate all
    // the other ObjId with the same Id.
    static PyObject * _pymoose_Neutral_destroy(_Neutral * self, PyObject * args)
    {
        PyObject * obj = NULL;
        if (!PyArg_ParseTuple(args, ":destroy")){
            return NULL;
        }
        self->_id.id.destroy();        
        Py_DECREF(obj);
        Py_RETURN_NONE;
    }

    
    static int _pymoose_Neutral_init(_Neutral * self, PyObject * args, PyObject * kwds)
    {
        static char * kwlist[] = {"type", "path", "dims"};
        char * path, *type;
        PyObject * dims = NULL;
        PyObject * src = NULL;
        // if (PyDict_Type.tp_init((PyObject *)self, args, kwds) < 0)
        //     return -1;
        
        if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss|O", kwlist, &type, &path, &dims)){
            if (!PyArg_ParseTuple(args, "O", &src)){
                return -1;
            } 
            _Neutral * src_cast = reinterpret_cast<_Neutral*>(src);
            if (!src_cast){
                PyErr_SetString(PyExc_TypeError, "Could not cast passed object to _Neutral.");
                return -1;
            }
            self->_id = src_cast->_id;
            return 0;
        }
        
        string trimmed_path(path);
        trimmed_path = trim(trimmed_path);
        size_t length = trimmed_path.length();
        if (length <= 0){
            PyErr_SetString(PyExc_ValueError, "path must be non-empty string.");
            return -1;
        }
        string trimmed_type = trim(string(type));
        if (trimmed_type.length() <= 0){
            PyErr_SetString(PyExc_ValueError, "type must be non-empty string.");
            return -1;
        }        

        //  paths ending with '/' should raise exception
        if ((length > 1) && (trimmed_path[length - 1] == '/')){
            PyErr_SetString(PyExc_ValueError, "Non-root path must not end with '/'");
            return -1;
        }
        vector <unsigned int> vec_dims;
        if (dims!= NULL && PySequence_Check(dims)){
            Py_ssize_t len = PySequence_Length(dims);
            for (Py_ssize_t ii = 0; ii < len; ++ ii){
                PyObject* dim = PySequence_GetItem(dims, ii);
                long dim_value = PyInt_AsLong(dim);
                if (dim_value == -1 && PyErr_Occurred()){
                    return -1;
                }
                vec_dims.push_back((unsigned int)dim_value);
            }                
        }
        if (vec_dims.empty()){
            vec_dims.push_back(1);
        }
        self->_id = Id(path);
        // If object does not exist, create new
        if ((self->_id == Id()) && (trimmed_path != "/") && (trimmed_path != "/root")){
            string parent_path;
            if (trimmed_path[0] != '/'){
                parent_path = getShell().getCwe().path();
            }
            size_t pos = trimmed_path.rfind("/");
            string name;
            if (pos != string::npos){
                name = trimmed_path.substr(pos+1);
                parent_path += "/";
                parent_path += trimmed_path.substr(0, pos+1);
            } else {
                name = trimmed_path;
            }
            self->_id = getShell().doCreate(string(type), Id(parent_path), string(name), vector<unsigned int>(vec_dims));
        } 
        return 0;            
    }// ! _pymoose_Neutral_init

    static PyObject * _pymoose_Neutral_getFieldType(_Neutral * self, PyObject * args)
    {
        char * fieldName = NULL;
        if (!PyArg_ParseTuple(args, "s", &fieldName)){
            return NULL;
        }
        string typeStr = getFieldType(self->_id.id, string(fieldName));
        if (typeStr.length() <= 0){
            PyErr_SetString(PyExc_ValueError, "Empty string for field type. Field name may be incorrect.");
            return NULL;
        }
        PyObject * type = PyString_FromString(typeStr.c_str());
        return type;
    }  // ! _pymoose_Neutral_getFieldType

    
    
    
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
