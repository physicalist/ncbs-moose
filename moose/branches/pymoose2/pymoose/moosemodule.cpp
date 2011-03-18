// moosemodule.cpp --- 
// 
// Filename: moosemodule.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 11:26:00 2011 (+0530)
// Version: 
// Last-Updated: Fri Mar 18 18:19:37 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 1427
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
#include "../basecode/header.h"
#include "../basecode/ObjId.h"

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "../basecode/ReduceBase.h"
#include "../basecode/ReduceMax.h"
#include "Shell.h"
#include "../utility/strutil.h"
#include "../scheduling/Tick.h"
#include "../scheduling/TickMgr.h"
#include "../scheduling/TickPtr.h"
#include "../scheduling/Clock.h"

extern void nonMpiTests(Shell *);
extern void mpiTests();
extern void processTests(Shell *);
extern void regressionTests();
extern unsigned int getNumCores();

using namespace std;
using namespace pymoose;



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

char shorttype(string ftype){
    static map<string, char> typemap;
    if (typemap.empty()){
        typemap["char"] = 'c';
        typemap["i"] = 'i';
        typemap["int"] = 'i';
        typemap["long"] = 'i';        
        typemap["f"] = 'f';
        typemap["float"] = 'f';
        typemap["double"] = 'f';
        typemap["ObjId"] = 'o';
        typemap["O"] = 'o';
        typemap["Object"] = 'o';
        typemap["short"] = 'i';
        typemap["s"] = 's';
        typemap["string"] = 's';
        typemap["str"] = 's';
        typemap["unsigned"] = 'u';
        typemap["unsigned int"] = 'u';
        typemap["uint"] = 'u';
        typemap["int_vector"] = 'v';
        typemap["float_vector"] = 'w';
        typemap["string_vector"] = 'x';
        typemap["double_vector"] = 'w';
        typemap["id_vector"] = 'y';
        typemap["oid_vector"] = 'z';
    }
    return typemap[ftype];
}

void * pymoose_Neutral_getField(pymoose_Neutral * instance, int index, string fname, char ftype)
{
    switch(ftype){
        case 'c':
            {
                char * ret = (char*)malloc(sizeof(char));
                *ret = Field< char >::get(ObjId(*(instance->id_), index), fname);
                return (void*)ret;
            }
        case 'i':
            {
                long* ret = (long*)malloc(sizeof(long));
                * ret = Field< int >::get(ObjId(*(instance->id_), index), fname);
                return (void*)ret;
            }
        case 'f':
            {
                double * ret = (double*)malloc(sizeof(double));
                *ret = Field< double >::get(ObjId(*(instance->id_), index), fname);
                return (void*) ret;
            }
        case 's':
            {
                string val = Field< string >::get(ObjId(*(instance->id_), index), fname);
                char ** ret = (char**)malloc(sizeof(char*));
                *ret = (char*)calloc(sizeof(char), val.length() + 1);
                strcpy(*ret, val.c_str());
                return (void*)ret;
            }
            break;
        case 'u':
            {
                unsigned long * ret = new unsigned long;
                *ret = Field<unsigned int>::get(ObjId(*(instance->id_), index), fname);
                return (void*)ret;
            }
            break;
        case 'v':
            {
                vector<int> val;
                Field<int>::getVec(Id(*(instance->id_)), string(fname), val);
                vector<int> * ret = new vector<int>(val);
                return (void*)ret;
            }
        case 'w':
            {
                vector<double> val;
                Field<double>::getVec(Id(*(instance->id_)), string(fname), val);
                vector<double> * ret = new vector<double>(val);
                return (void*)ret;
            }
        default:
            return NULL;
    };
}
    
int pymoose_Neutral_setField(pymoose_Neutral * instance, int index, string fname, char ftype, void * value)
{
    switch(ftype){
        case 'c':
            {
                char  c_val = *((char*)value);
                Field<char>::set(ObjId(*(instance->id_), index), fname, c_val);
                break;
            }                
        case 'i':
            {
                int c_val = *((int*)value);
                if ((c_val == -1) && PyErr_Occurred()){
                    return NULL;

                }
                Field<int>::set(ObjId(*(instance->id_), index), fname, c_val);
                break;
            }
        case 'f':
            {
                double c_val = *((double*)value);
                Field<double>::set(ObjId(*(instance->id_), index), fname, c_val);
                break;
            }
        case 's':
            {
                char * c_val = *((char**)value);
                if (c_val){
                    Field<string>::set(ObjId(*(instance->id_), index), string(fname), string(c_val));
                } else {
                    return 0;
                }
                break;
            }
        default:
            return 0;            
    };
    return 1;
}
// 
// C wrappers for C++ classes
// This is used by Python
extern "C" {
    static Shell * __shell = NULL;
    static PyObject * MooseError;
    static PyObject * SingleThreaded;
    static PyObject * NumNodes;
    static PyObject * NumCores;
    static PyObject * MyNode;
    static PyObject * Infinite;
    
    static PyObject * _pymoose_Neutral_new(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_delete(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_id(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_path(PyObject* dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_setattr(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_getattr(PyObject * dummy, PyObject * args);
    static PyObject * _pymoose_Neutral_destroy(PyObject * dummy, PyObject * args);
    // static PyObject * initShell(PyObject * dummy, PyObject * args, PyObject * kwdict);
    static Shell * getShell();
    static void initEnvironment();
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
        {"_pymoose_Neutral_destroy", _pymoose_Neutral_destroy, METH_VARARGS,
         "destroy the underlying moose element"},
        {NULL, NULL, 0, NULL}        /* Sentinel */
    };


    /* module initialization */
    PyMODINIT_FUNC init_moose()
    {
        PyObject *moose_module = Py_InitModule("_moose", MooseMethods);
        if (moose_module == NULL)
            return;
        MooseError = PyErr_NewException("moose.error", NULL, NULL);
        Py_INCREF(MooseError);
        PyModule_AddObject(moose_module, "error", MooseError);
        SingleThreaded = PyInt_FromLong(1);
        Py_INCREF(SingleThreaded);
        PyModule_AddObject(moose_module, "SingleThreaded", SingleThreaded);
        NumCores = PyInt_FromLong(1);
        Py_INCREF(NumCores);
        PyModule_AddObject(moose_module, "NumCores", NumCores);
        NumNodes = PyInt_FromLong(1);
        Py_INCREF(NumNodes);
        PyModule_AddObject(moose_module, "NumNodes", NumNodes);
        MyNode = PyInt_FromLong(0);
        Py_INCREF(MyNode);
        PyModule_AddObject(moose_module, "MyNode", MyNode);
        Infinite = PyInt_FromLong(0);
        Py_INCREF(Infinite);
        PyModule_AddObject(moose_module, "Infinite", Infinite);
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        __shell = getShell();
        PyGILState_Release(gstate);
    }

    // static PyObject * initShell(PyObject * dummy, PyObject * args, PyObject * kwords)
    // {
    //     int isSingleThreaded, numNodes, numCores, isInfinite;
    //     static char * kwlist[] = {"singlethreaded", "numcores", "numnodes"};
    //     if (__shell){
    //         PyErr_SetString(PyExc_RuntimeWarning, "Shell already initialized.");
    //         Py_RETURN_NONE;
    //     }
    //     if (!PyArg_ParseTupleAndKeywords(args, kwords, "|iiii", kwlist, &isSingleThreaded, &numCores, &numNodes, &isInfinite)){
    //         return NULL;
    //     }
    //     __shell = getShell();
    //     return reinterpret_cast<PyObject*>(__shell);
    // }
    static void initEnvironment()
    {
        int isSingleThreaded, numNodes, numCores, isInfinite, myNode;
        /**
         * Here we allow the user to override the automatic identification
	 * of processor configuration
	 */
        char * env_opt = NULL;
        env_opt = getenv("SINGLETHREADED");
        if (env_opt){            
            if(!(istringstream(string(env_opt)) >> isSingleThreaded)){
                isSingleThreaded = 0;
            }
        }
        env_opt = getenv("INFINITE");
        if (env_opt){
            if (!(istringstream(env_opt) >> isInfinite)){
                isInfinite = 0;
            }
        }
        env_opt = getenv("NODES");
        if (env_opt){
            if (!(istringstream(env_opt) >> numNodes)){
                numNodes = 1;
            }
        }
        env_opt = getenv("CORES");
        if (env_opt){
            if (!(istringstream(env_opt) >> numCores)){
                numCores = getNumCores();
            }
        }
        if (isSingleThreaded < 0){            
            isSingleThreaded = 0;
        }
        if (numCores < 0){
            numCores = getNumCores();
        }
        if (numNodes < 0){
            numNodes = 1;
        }
        if (isInfinite < 0){
            isInfinite = 0;
        }
        if (myNode < 0){
            myNode = 0;
        }
        SingleThreaded = PyInt_FromLong(isSingleThreaded);
        NumCores = PyInt_FromLong(numCores);
        NumNodes = PyInt_FromLong(numNodes);
        MyNode = PyInt_FromLong(myNode);
        Infinite = PyInt_FromLong(isInfinite);
        cout << "Creating MOOSE Shell on node " << myNode
             << ", numNodes = " << numNodes
             << ", numCores = " << numCores 
             << ", multithreaded = " << !isSingleThreaded << endl;

    }
    static Shell * getShell()
    {
        static Shell * shell = NULL;
        if (shell){
            return shell;
        }
                
        long isSingleThreaded = PyInt_AsLong(SingleThreaded);
        long numCores = PyInt_AsLong(NumCores);
        long numNodes = PyInt_AsLong(NumNodes);
        long myNode = PyInt_AsLong(MyNode);
        long isInfinite = PyInt_AsLong(Infinite);
        
#ifdef USE_MPI
	int provided;
	// OpenMPI does not use argc or argv.
	// unsigned int temp_argc = 1;
	//MPI_Init_thread( &temp_argc, &argv, MPI_THREAD_SERIALIZED, &provided );
	MPI_Init_thread( &argc, &argv, MPI_THREAD_SERIALIZED, &provided );
	MPI_Comm_size( MPI_COMM_WORLD, &numNodes );
	MPI_Comm_rank( MPI_COMM_WORLD, &myNode );
	if ( provided < MPI_THREAD_SERIALIZED && myNode == 0 ) {
            cout << "Warning: This MPI implementation does not like multithreading: " << provided << "\n";
	}
	// myNode = MPI::COMM_WORLD.Get_rank();
#endif
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
        return shell;    
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
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        pymoose_Neutral_delete(dummy, object);
        PyGILState_Release(gstate);

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
        char _ftype = shorttype(ftype);
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        void * ret = pymoose_Neutral_getField(instance, index, field_str, _ftype);
        PyGILState_Release(gstate);
        PyObject * pyret = NULL;
        switch(_ftype){
            case 'c':
                {
                    pyret = Py_BuildValue("c", *((char*)ret));
                    break;
                }
            case 'i':
                {
                    pyret = Py_BuildValue("i", *((long*)ret));
                }
            case 'f':
                {
                    pyret = Py_BuildValue("f", *((double*)ret));
                }
                break;
            case 's':
                {
                    pyret = Py_BuildValue("s", *((char *)ret));                
                }
                break;
            case 'u':
                {
                    pyret = Py_BuildValue("i", *((unsigned long*)ret));                    
                }
                break;
            case 'v':
                {
                    vector <long>& val = *((vector<long>*)ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (int ii = 0; ii < val.size(); ++ ii ){
                        if (!PyTuple_SetItem(pyret, (Py_ssize_t)ii, PyInt_FromLong(val[ii]))){
                            pyret = NULL;
                            break;
                        }                        
                    }
                }
                break;
            case 'w':
                {
                    vector<double>& val = *((vector<double> *) ret);
                    PyObject * pyret = PyTuple_New((Py_ssize_t)val.size());
                    for (int ii = 0; ii < ((vector<long>*)ret)->size(); ++ ii){
                        if (!PyTuple_SetItem(pyret, (Py_ssize_t)ii, PyFloat_FromDouble(val[ii]))){
                            pyret = NULL;
                            break;
                        }
                    }
                }
                break;
            default:
                {
                    PyErr_SetString(PyExc_TypeError, string("Invalid field type: " + ftype_str).c_str());
                    pyret = NULL;
                }
        }
        delete ret;
        return pyret;        
    }
    static PyObject * _pymoose_Neutral_setattr(PyObject * dummy, PyObject * args)
    {
        PyObject * obj = NULL;
        PyObject * value;
        char * field;
        char * ftype;
        int index = 0;
        if (!PyArg_ParseTuple(args, "OssO|i", &obj, &field, &ftype, &value, &index)){
            return NULL;
        }
        pymoose_Neutral * instance = reinterpret_cast<pymoose_Neutral*>(obj);
        if (!instance){
            PyErr_SetString(PyExc_TypeError, "Argument cannot be cast to pymoose_Neutral pointer.");
            return NULL;
        }
        string field_str(field);
        string ftype_str(ftype);
        void * value_ptr = NULL;
        char _ftype = shorttype(ftype);
        switch(_ftype){
            case 'c':
                {
                    char * _value_ptr = (char*)malloc(sizeof(char));
                    char * pychar = PyString_AsString(value);
                    
                    if (pychar){
                        *_value_ptr = pychar[0];
                        value_ptr = _value_ptr;
                    } else {
                        return NULL;
                    }
                    break;
                }
            case 'i':
                {
                    long * _value_ptr = (long*)malloc(sizeof(long));
                    long pyint = PyInt_AsLong(value);
                    if ((pyint == -1) && (PyErr_Occurred())){
                        return NULL;
                    } else {
                        *_value_ptr = pyint;
                        value_ptr = _value_ptr;
                    }
                    break;
                }
            case 'f':
                {
                    double * _value_ptr = (double*) malloc(sizeof(double));
                    double pydouble = PyFloat_AsDouble(value);
                    * _value_ptr = pydouble;
                    value_ptr = _value_ptr;
                    break;
                }
            case 's':
                {
                    char **_value_ptr = (char**)malloc(sizeof(char*));
                    char * pystr = PyString_AsString(value);
                    if (pystr){
                        *_value_ptr = (char*)calloc(sizeof(char), strlen(pystr) + 1);
                        strcpy(*_value_ptr, pystr);
                        value_ptr = _value_ptr;
                    } else {
                        return NULL;
                    }
                    break;
                }
            default:
                break;
        }
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        int ret = pymoose_Neutral_setField(instance, index, field, _ftype, value_ptr);
        PyGILState_Release(gstate);
        if (ret){
            Py_RETURN_NONE;
        } else {
            PyErr_SetString(PyExc_TypeError, "The specified field type is not valid.");
            return NULL;
        }
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
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        instance->id_->destroy();
        PyGILState_Release(gstate);
        Py_DECREF(obj);
        Py_RETURN_NONE;
    }
} // end extern "C"


int main(int argc, char* argv[])
{
    for (int ii = 0; ii < argc; ++ii){
        cout << "ARGV: " << argv[ii];
    }
    cout << endl;
    Py_SetProgramName(argv[0]);
    PyEval_InitThreads();
    Py_Initialize();
    init_moose();
    return 0;
}

// 
// moosemodule.cpp ends here
