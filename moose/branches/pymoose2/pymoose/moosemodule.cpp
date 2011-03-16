// moosemodule.cpp --- 
// 
// Filename: moosemodule.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 11:26:00 2011 (+0530)
// Version: 
// Last-Updated: Wed Mar 16 17:20:12 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 893
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
#include "ObjId.h"
#include "ReduceBase.h"
#include "ReduceMax.h"
#include "Shell.h"
#include "../utility/strutil.h"
#include "../scheduling/Tick.h"
#include "../scheduling/TickMgr.h"
#include "../scheduling/TickPtr.h"
#include "../scheduling/Clock.h"

extern Id init(int argc, char **argv);
extern void nonMpiTests(Shell *);
extern void mpiTests();
extern void processTests(Shell *);
extern void regressionTests();

using namespace std;
using namespace pymoose;

static Shell * getShell()
{
    static Shell * shell = NULL;
    if (!shell){
        // Going with all these dafaults to start with
        int isSingleThreaded = 1;
        int numCores = 1;
        int numNodes = 1;
        int myNode = 0;
        bool isInfinite = 0;
        
        cout << "getShell: Creating the shell instance" << endl;

        // Now it is copied over from main.cpp: init()
        Msg::initNull();
        Id shellId;
        vector <unsigned int> dims;
        dims.push_back(1);
        Element * shellE = new Element(shellId, Shell::initCinfo(), "root", dims, 1);
        Id clockId = Id::nextId();
        shell = reinterpret_cast<Shell*>(shellId.eref().data());
        shell->setShellElement(shellE);
        shell->setHardware(isSingleThreaded, numCores, numNodes, myNode);
        shell->loadBalance();

        new Element(clockId, Clock::initCinfo(), "clock", dims, 1);
        Id tickId( 2 );
        assert(tickId() != 0);
	assert( tickId.value() == 2 );
	assert( tickId()->getName() == "tick" ) ;

	assert ( shellId == Id() );
	assert( clockId == Id( 1 ) );
	assert( tickId == Id( 2 ) );

	/// Sets up the Elements that represent each class of Msg.
	Msg::initMsgManagers();

	shell->connectMasterMsg();

	Shell::adopt( shellId, clockId );
	while ( isInfinite ) // busy loop for debugging under gdb and MPI.
            ;        
    }
    return shell;    
}

PyMooseBase::PyMooseBase()
{
}
PyMooseBase::~PyMooseBase()
{
}

pymoose_Neutral::pymoose_Neutral(Id id)
{
    this->id_ = new Id(id);
}
pymoose_Neutral::~pymoose_Neutral()
{
    delete this->id_;
}

// Class definitions end here


// 
// C wrappers for C++ classes
// This is used by Python
extern "C" {
    static PyObject * MooseError;    
    static PyObject * moose_test_dummy(PyObject* dummy, PyObject* args);
    static PyObject * _pymoose_Neutral_new(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_delete(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_id(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_path(PyObject* dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_setattr(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_getattr(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_destroy(PyObject * dummy, PyObject * args);
    // static PyObject* __shell;
    /**
     * Method definitions.
     */
    static PyMethodDef MooseMethods[] = {
        {"test_dummy",  moose_test_dummy, METH_VARARGS,
         "A test function."},
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
        {"_pymoose_Neutral_destroy", _pymoose_Neutral_destroy, METH_VARARGS,
         "destroy the underlying moose element"},
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
    PyMODINIT_FUNC init_moose()
    {
        PyObject *moose_module = Py_InitModule("_moose", MooseMethods);
        if (moose_module == NULL)
            return;
        MooseError = PyErr_NewException("moose.error", NULL, NULL);
        Py_INCREF(MooseError);
        PyModule_AddObject(moose_module, "error", MooseError);
        Shell * __shell = getShell();
    }

    pymoose_Neutral* pymoose_Neutral_new(PyObject * dummy, PyObject * args)
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
	Id id = Id(trimmed_path);
        if ((id == Id()) && (trimmed_path != "/")) { // object does not exist
            size_t pos = trimmed_path.rfind('/');
            string parent_path;
            string name;
            if (pos != string:: npos){
                parent_path = trimmed_path.substr(0, pos);
                name = trimmed_path.substr(pos);
            } else {
                parent_path = getShell()->getCwe().path();
                name = trimmed_path;
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
            id = getShell()->doCreate(string(trimmed_type),
                                      Id(parent_path),
                                      string(name),
                                      vector<unsigned int>(vec_dims));
        }
        pymoose_Neutral* ret = new pymoose_Neutral(id);
        return ret;
    }

    static PyObject * _pymoose_Neutral_new(PyObject * dummy, PyObject * args)
    {
        PyObject * ret = (PyObject *)(pymoose_Neutral_new(dummy, args));
        return ret;
    }

    void pymoose_Neutral_delete(PyObject* dummy, PyObject* object)
    {
        pymoose_Neutral * moose_object = reinterpret_cast<pymoose_Neutral*>(object);
        delete moose_object;
    }

    static PyObject * _pymoose_Neutral_delete(PyObject * dummy, PyObject * args)
    {
        PyObject * object = NULL;
        if (!PyArg_ParseTuple(args, "O", &object)){
                return NULL;
        }
        pymoose_Neutral_delete(dummy, object);
        Py_RETURN_NONE;
    }
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
        unsigned int id = instance->id_->value();
        PyObject * ret = Py_BuildValue("I", id);
        return ret;
    }

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
        string path = instance->id_->path();
        PyObject * ret = Py_BuildValue("s", path.c_str());
        return ret;
    }

    static PyObject * _pymoose_Neutral_getattr(PyObject * dummy, PyObject * args)
    {
        PyObject * obj = NULL;
        const char * field = NULL;
        const char * ftype = NULL;
        const int index = 0;
        if (!PyArg_ParseTuple(args, "Oss|i", &obj, &field, &ftype, &index)){
            return NULL;
        }
        pymoose_Neutral * instance = reinterpret_cast<pymoose_Neutral*>(obj);
        if (!instance){
            PyErr_SetString(PyExc_TypeError, "Argument cannot be cast to pymoose_Neutral pointer.");
            return NULL;
        }
        string ftype_str(ftype);
        string field_str(field);
        if (ftype_str == "ObjId"){
            ObjId objId = Field< ObjId >::get(ObjId(*(instance->id_), index), field_str);
        } else if (ftype_str == "int"){
            int ret = Field< int >::get(ObjId(*(instance->id_), index), field_str);
            return Py_BuildValue("i", ret);
        } else if (ftype_str == "float" || ftype_str == "double"){
            double ret = Field< double >::get(ObjId(*(instance->id_), index), field_str);
            return Py_BuildValue("f", ret);
        } else if (ftype_str == "str" || ftype_str == "string"){
            string ret = Field<string>::get(ObjId(Id(*(instance->id_)), index), field_str);
            return Py_BuildValue("s", ret.c_str());
        } else if (ftype_str == "int_vector" ){
            vector<int> ret;
            PyObject * pyret;
            string field_str(field); 
            Field< int >::getVec(Id(*(instance->id_)), field_str, ret);
            pyret = PyTuple_New((Py_ssize_t)ret.size());
            for (int ii = 0; ii < ret.size(); ++ ii){
                if (PyTuple_SetItem(pyret, (Py_ssize_t)ii, PyInt_FromLong(ret[ii])))
                    return NULL;
            }
            return pyret;
        } else if (ftype_str == "float_vector" || ftype_str == "double_vector"){
            vector<double> ret;
            PyObject * pyret;
            Field< double >::getVec(Id(*(instance->id_)), field_str, ret);
            pyret = PyTuple_New((Py_ssize_t)ret.size());
            for (int ii = 0; ii < ret.size(); ++ ii){
                if (PyTuple_SetItem(pyret, (Py_ssize_t)ii, PyFloat_FromDouble(ret[ii])))
                    return NULL;
            }
            return pyret;
        }else if (ftype_str == "string_vector"){
            vector<string> ret;
            PyObject * pyret;
            Field< string >::getVec(Id(*(instance->id_)), field_str, ret);
            pyret = PyTuple_New((Py_ssize_t)ret.size());
            for (int ii = 0; ii < ret.size(); ++ ii){
                if (PyTuple_SetItem(pyret, (Py_ssize_t)ii, PyString_FromString(ret[ii].c_str())))
                    return NULL;
            }
            return pyret;
        }
        Py_RETURN_NONE;
    }
    static PyObject * _pymoose_Neutral_setattr(PyObject * dummy, PyObject * args)
    {
        cout << "Not yet implemented" << endl;
        Py_RETURN_NONE;
    }
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
        instance->id_->destroy();
        Py_DECREF(obj);
        Py_RETURN_NONE;
    }

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
