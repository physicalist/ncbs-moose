// moosemodule.h --- 
// 
// Filename: moosemodule.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Thu Mar 10 17:11:06 2011 (+0530)
// Version: 
// Last-Updated: Sun Apr 22 22:42:25 2012 (+0530)
//           By: Subhasis Ray
//     Update #: 864
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
       _Id wraps the Id class - where each element is identified by Id
    */
    typedef struct {
        PyObject_HEAD
        Id id_;
    } _Id;
    /**
       _ObjId wraps the subelements of a Id - identified by
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
        ObjId oid_;
    } _ObjId;

    typedef struct {
        PyObject_HEAD
        ObjId owner;
        char * name;
    } _Field;
    
    //////////////////////////////////////////
    // Methods for Id class
    //////////////////////////////////////////
    static int moose_Id_init(_Id * self, PyObject * args, PyObject * kwargs);
    static long moose_Id_hash(_Id * self);
    
    static PyObject * moose_Id_repr(_Id * self);
    static PyObject * moose_Id_str(_Id * self);
    static PyObject * moose_Id_delete(_Id * self, PyObject * args);
    static PyObject * moose_Id_getValue(_Id * self, PyObject * args);
    static PyObject * moose_Id_getPath(_Id * self, PyObject * args);
    /* Id functions to allow part of sequence protocol */
    static Py_ssize_t moose_Id_getLength(_Id * self);
    static PyObject * moose_Id_getItem(_Id * self, Py_ssize_t index);
    static PyObject * moose_Id_getSlice(_Id * self, PyObject * args);    
    static PyObject * moose_Id_getShape(_Id * self, PyObject * args);    
    static PyObject * moose_Id_richCompare(_Id * self, PyObject * args, int op);
    static int moose_Id_contains(_Id * self, PyObject * args);
    ///////////////////////////////////////////
    // Methods for ObjId class
    ///////////////////////////////////////////
    static int moose_ObjId_init(PyObject * self, PyObject * args, PyObject * kwargs);
    static long moose_ObjId_hash(_ObjId * self);
    static PyObject * moose_ObjId_repr(_ObjId * self);
    static PyObject * moose_ObjId_getattro(_ObjId * self, PyObject * attr);
    static PyObject * moose_ObjId_getField(_ObjId * self, PyObject * args);
    static int moose_ObjId_setattro(_ObjId * self, PyObject * attr, PyObject * value);
    static PyObject * moose_ObjId_setField(_ObjId * self, PyObject * args);
    static PyObject * moose_ObjId_getLookupField(_ObjId * self, PyObject * args);
    static PyObject * moose_ObjId_setLookupField(_ObjId * self, PyObject * args);
    static PyObject * moose_ObjId_setDestField(PyObject * self, PyObject * args);
    static PyObject * moose_ObjId_getFieldNames(_ObjId * self, PyObject * args);
    static PyObject * moose_ObjId_getFieldType(_ObjId * self, PyObject * args);
    static PyObject * moose_ObjId_getDataIndex(_ObjId * self, PyObject * args);
    static PyObject * moose_ObjId_getFieldIndex(_ObjId * self, PyObject * args);
    static PyObject * moose_ObjId_getNeighbors(_ObjId * self, PyObject * args);
    static PyObject * moose_ObjId_getId(_ObjId * self, PyObject * args);
    static PyObject * moose_ObjId_connect(_ObjId * self, PyObject * args);
    static int moose_ObjId_compare(_ObjId * self, PyObject * args);

    ////////////////////////////////////////////
    // Methods for LookupField
    ////////////////////////////////////////////
    static int moose_Field_init(_Field * self, PyObject * args, PyObject * kwds);
    static long moose_Field_hash(_Field * self);
    static PyObject * moose_Field_repr(_Field * self);
    static PyObject * moose_LookupField_getItem(_Field * self, PyObject * key);
    static int moose_LookupField_setItem(_Field * self, PyObject * key, PyObject * value);
    
    
    ////////////////////////////////////////////////
    // static functions to be accessed from Python
    ////////////////////////////////////////////////


    // The following are global functions
    static PyObject * moose_useClock(PyObject * dummy, PyObject * args);
    static PyObject * moose_setClock(PyObject * dummy, PyObject * args);
    static PyObject * moose_start(PyObject * dummy, PyObject * args);
    static PyObject * moose_reinit(PyObject * dummy, PyObject * args);
    static PyObject * moose_stop(PyObject * dummy, PyObject * args);
    static PyObject * moose_isRunning(PyObject * dummy, PyObject * args);
    static PyObject * moose_exists(PyObject * dummy, PyObject * args);
    static PyObject * moose_loadModel(PyObject * dummy, PyObject * args);
    static PyObject * moose_setCwe(PyObject * dummy, PyObject * args);
    static PyObject * moose_getCwe(PyObject * dummy, PyObject * args);
    static PyObject * moose_copy(PyObject * dummy, PyObject * args, PyObject * kwargs);
    static PyObject * moose_move(PyObject * dummy, PyObject * args);
    static PyObject * moose_delete(PyObject * dummy, PyObject * args);
    static PyObject * moose_connect(PyObject * dummy, PyObject * args);
    static PyObject * moose_getFieldDict(PyObject * dummy, PyObject * args);
    static PyObject * moose_getField(PyObject * dummy, PyObject * args);
    static PyObject * moose_syncDataHandler(PyObject * dummy, _Id * target);
    static PyObject * moose_seed(PyObject * dummy, PyObject * args);
    static PyObject * moose_wildcardFind(PyObject * dummy, PyObject * args);
    // This should not be required or accessible to the user. Put here
    // for debugging threading issue.
    static PyObject * moose_quit(PyObject * dummy);


    static PyObject * getLookupField(ObjId oid, char * fieldName, PyObject * key);
    static int setLookupField(ObjId oid, char * fieldName, PyObject * key, PyObject * value);
    static int defineClass(PyObject * module_dict, string class_name);
    static int define_destFinfos(string class_name);
    static int defineAllClasses(PyObject* module_dict);
    static int define_lookupFinfos(string class_name);
    static PyObject * moose_ObjId_get_lookupField_attr(PyObject * self, void * closure);
    static PyObject * moose_ObjId_get_destField_attr(PyObject * self, void * closure);
    static PyObject * _setDestField(ObjId oid, PyObject * args);
    PyMODINIT_FUNC init_moose();


    int inner_getFieldDict(Id classId, string finfoType, vector<string>& fields, vector<string>& types); 


    
} //!extern "C"

#define PYSEQUENCE_TO_VECTOR(BASETYPE, SEQUENCE){                       \
        Py_ssize_t length = PySequence_Length(SEQUENCE);            \
        vector < BASETYPE > * RET;                                       \
        BASETYPE * value;                                                \
        RET = new vector<BASETYPE>();                            \
        for (unsigned int ii = 0; ii < length; ++ii){               \
            PyObject * item = PySequence_GetItem(SEQUENCE, ii);     \
            if (item == NULL){                                      \
                ostringstream error;                                \
                error << "Item # " << ii << " is NULL";                 \
                PyErr_SetString(PyExc_TypeError, error.str().c_str());  \
            } else {                                                    \
                value = (BASETYPE*)to_cpp< BASETYPE >(item);         \
                if (value == NULL){                                     \
                    PyErr_SetString(PyExc_TypeError, "Cannot handle sequence of type "#BASETYPE); \
                } else {                                                \
                    RET->push_back(*value);                             \
                    delete value;                                       \
                }                                                       \
            }                                                           \
        }                                                               \
        return RET;                                                     \
    }


/// Converts PyObject to C++ object
/// Returns a pointer to the converted object. Deallocation is caller responsibility
template <class A> void * to_cpp(PyObject * object)
{
    if (typeid(A) == typeid(int)){
        int * ret = new int();
        * ret = PyInt_AsLong(object);
        return (void*)ret;
    } else if (typeid(A) == typeid(long)){
        long v = PyInt_AsLong(object);
        long * ret = new long();
        *ret = v;
        return (void*)ret;
    } else if (typeid(A) == typeid(short)){
        short v = PyInt_AsLong(object);
        short * ret = new short();
        *ret = v;
        return (void*)ret;
    } else if (typeid(A) == typeid(float)){
        float v = (float)PyFloat_AsDouble(object);
        if ( v == -1.0 && PyErr_Occurred()){
            PyErr_SetString(PyExc_TypeError, "Expected a sequence of floating point numbers.");
        } else {
            float * ret = new float();
            *ret = v;
            return (void*)ret;
        }
    } else if (typeid(A) == typeid(double)){
        double v = PyFloat_AsDouble(object);
        if ( v == -1.0 && PyErr_Occurred()){
            PyErr_SetString(PyExc_TypeError, "Expected a sequence of floating point numbers.");
        } else {
            double * ret = new double();
            *ret = v;
            return (void*)ret;
        }
    } else if (typeid(A) == typeid(string)) {
        char* tmp = PyString_AsString(object);
        if (tmp == NULL){
            return NULL;
        }        
        string * ret = new string(tmp);
        return (void*)ret;
    } else if (typeid(A) == typeid(unsigned int)) {
        unsigned int v = PyInt_AsUnsignedLongMask(object);
        unsigned int * ret = new unsigned int();
        *ret = v;
        return (void*)ret;
    } else if (typeid(A) == typeid(unsigned long)) {
        unsigned long v = PyInt_AsUnsignedLongMask(object);
        unsigned long * ret = new unsigned long();
        *ret = v;
        return (void*)ret;
    } else if (typeid(A) == typeid(Id))
    {
        _Id * value = (_Id*)object;
        if (value != NULL){
            Id * ret = new Id();
            * ret = value->id_;
        return (void*)ret;
        }
    } else if (typeid(A) == typeid(ObjId)){
        _ObjId * value = (_ObjId*)object;
        if (value != NULL){
            ObjId * ret = new ObjId();
            * ret = value->oid_;
            return (void*)ret;
        }
    } else if (typeid(A) == typeid(vector<int>)){
        PYSEQUENCE_TO_VECTOR( int, object)
    } else if (typeid(A) == typeid(vector<unsigned int>)){
        PYSEQUENCE_TO_VECTOR( unsigned int, object)
    } else if (typeid(A) == typeid(vector<short>)){
        PYSEQUENCE_TO_VECTOR( short, object)
    } else if (typeid(A) == typeid(vector<long>)){
        PYSEQUENCE_TO_VECTOR( long, object)
    } else if (typeid(A) == typeid(vector<unsigned long>)){
        PYSEQUENCE_TO_VECTOR( unsigned long, object)
    } else if (typeid(A) == typeid(vector<float>)){
        PYSEQUENCE_TO_VECTOR( float, object)
    } else if (typeid(A) == typeid(vector<double>)){
        PYSEQUENCE_TO_VECTOR( double, object)
    } else if (typeid(A) == typeid(vector<string>)){
        PYSEQUENCE_TO_VECTOR( string, object)
    } else if (typeid(A) == typeid(vector<ObjId>)){
        PYSEQUENCE_TO_VECTOR( ObjId, object)        
    } else if (typeid(A) == typeid(vector<Id>)){
        PYSEQUENCE_TO_VECTOR( Id, object)
    }
    return NULL;
}

/// Set a destinfo that takes a vector argument
template <class A> inline PyObject* _set_vector_destFinfo(ObjId obj, string fieldName, int argIndex, PyObject * value)
{
    ostringstream error;
    if (!PySequence_Check(value)){                                  
        PyErr_SetString(PyExc_TypeError, "For setting vector field, specified value must be a sequence." );
        return NULL;
    }
    if (argIndex > 0){
        PyErr_SetString(PyExc_TypeError, "Can handle only single-argument functions with vector argument." );
        return NULL;
    }            
    vector<A> * _value = (vector <A> *)to_cpp < vector <A> >(value);
    if (_value == NULL){
        return NULL;
    }
    bool ret = SetGet1< vector < A > >::set(obj, fieldName, *_value);
    delete _value;
    if (ret){
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

/// The circus with value_ptr is to allow Id, ObjId to be allocated.
template <class KeyType> inline PyObject * lookup_value(const ObjId& oid, string fname, char value_type_code, char key_type_code, PyObject * key, void * value_ptr=NULL)
{
    PyObject * ret = NULL;
    KeyType * cpp_key = (KeyType *)to_cpp<KeyType>(key);
    if (cpp_key == NULL){
        return NULL;
    }
    string value_type_str = string(1, value_type_code);
    switch (value_type_code){
        case 'b': { // boolean is a special case that PyBuildValue does not handle
            bool value = LookupField < KeyType, bool > ::get(oid, fname, *cpp_key);
            if (value){
                Py_RETURN_TRUE;
            } else {
                Py_RETURN_FALSE;
            }
        }
        case 'c': {
            char value = LookupField < KeyType, char > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }            
        case 'h': {
            short value = LookupField < KeyType, short > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }            
        case 'H': {
            unsigned short value = LookupField < KeyType, unsigned short > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }            
        case 'i': {
            int value = LookupField < KeyType, int > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }            
        case 'I': {
            unsigned int value = LookupField < KeyType, unsigned int > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }            
        case 'l': {
            long value = LookupField < KeyType, long > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }                        
        case 'k': {
            unsigned long value = LookupField < KeyType, unsigned long > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }
#ifdef HAVE_LONG_LONG            
        case 'L': {
            long long value = LookupField < KeyType, long long > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }                        
        case 'K': {
            unsigned long long value = LookupField < KeyType, unsigned long long > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }
#endif
        case 'd': {
            double value = LookupField < KeyType, double > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }                        
        case 'f': {
            float value = LookupField < KeyType, float > ::get(oid, fname, *cpp_key);
            ret = Py_BuildValue(value_type_str.c_str(), value);
            break;
        }
        case 'x': {
            assert(value_ptr != NULL);
            ((_Id*)value_ptr)->id_ = LookupField < KeyType, Id > ::get(oid, fname, *cpp_key);
            ret = (PyObject*)value_ptr;
            break;
        }
        case 'y': {
            assert(value_ptr != NULL);
            ((_ObjId*)value_ptr)->oid_ = LookupField < KeyType, Id > ::get(oid, fname, *cpp_key);
            ret = (PyObject*)value_ptr;
            break;
        }
        default:
            PyErr_SetString(PyExc_TypeError, "invalid value type");
    }
    delete cpp_key;
    return ret;
}

/// The circus with value_ptr is to allow Id, ObjId to be allocated.
template <class KeyType> inline int set_lookup_value(const ObjId& oid, string fname, char value_type_code, char key_type_code, PyObject * key, PyObject * value_obj)
{
    bool success = false;
    KeyType *cpp_key = (KeyType*)to_cpp<KeyType>(key);
    if (cpp_key == NULL){
        return -1;
    }
#define SET_LOOKUP_VALUE( TYPE )                                        \
    {                                                                   \
        TYPE * value = (TYPE*)to_cpp<TYPE>(value_obj);                 \
            if (value){                                                 \
                success = LookupField < KeyType, TYPE > ::set(oid, fname, *cpp_key, *value); \
                delete value;                                           \
                delete cpp_key;                                         \
            }                                                           \
            break;                                                      \
    }
    
    string value_type_str = string(1, value_type_code);
    switch (value_type_code){
        case 'b':
            SET_LOOKUP_VALUE(bool)        
        case 'c':
            SET_LOOKUP_VALUE(char)
        case 'h':
            SET_LOOKUP_VALUE(short)
        case 'H':
            SET_LOOKUP_VALUE(unsigned short)
        case 'i':
            SET_LOOKUP_VALUE(int)
        case 'I':
            SET_LOOKUP_VALUE(unsigned int)
        case 'l':
            SET_LOOKUP_VALUE(long)
                    
        case 'k': 
            SET_LOOKUP_VALUE(unsigned long)
#ifdef HAVE_LONG_LONG
        case 'L':
            SET_LOOKUP_VALUE(long long)
        case 'K': 
            SET_LOOKUP_VALUE(unsigned long long);
#endif
        case 'd':
            SET_LOOKUP_VALUE(double)
        case 'f': 
            SET_LOOKUP_VALUE(float)
        case 's':
            SET_LOOKUP_VALUE(string)
        case 'x':
            SET_LOOKUP_VALUE(Id)
        case 'y':
            SET_LOOKUP_VALUE(ObjId)
        default:
            PyErr_SetString(PyExc_TypeError, "invalid value type");
    }
    if (success){
        return 0;
    } else {
        return -1;
    }
}

#endif // _MOOSEMODULE_H

// 
// moosemodule.h ends here
