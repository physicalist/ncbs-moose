// mfield.cpp --- 
// 
// Filename: mfield.cpp
// Description: 
// Author: 
// Maintainer: 
// Created: Mon Jul 22 17:03:03 2013 (+0530)
// Version: 
// Last-Updated: Tue Jul 23 16:11:50 2013 (+0530)
//           By: subha
//     Update #: 13
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
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth
// Floor, Boston, MA 02110-1301, USA.
// 
// 

// Code:

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include <structmember.h> // This defines the type id macros like T_STRING
#include "numpy/arrayobject.h"

#include <iostream>
#include <typeinfo>
#include <cstring>
#include <map>
#include <ctime>

#include "../basecode/header.h"
#include "../basecode/Id.h"
#include "../basecode/ObjId.h"
#include "../utility/utility.h"
#include "../randnum/randnum.h"
#include "../shell/Shell.h"

#include "moosemodule.h"

using namespace std;

extern "C" {

    extern PyTypeObject ObjIdType;
    extern PyTypeObject IdType;

    // Does not get called at all by PyObject_New. See:
    // http://www.velocityreviews.com/forums/t344033-pyobject_new-not-running-tp_new-for-iterators.html
    // static PyObject * moose_Field_new(PyTypeObject *type,
    //                                   PyObject *args, PyObject *kwds)
    // {
    //     _Field *self = NULL;
    //     self = (_Field*)type->tp_alloc(type, 0);
    //     if (self != NULL){            
    //         self->name = NULL;
    //         self->owner = ObjId::bad;
    //     }        
    //     return (PyObject*)self;
    // }
    
    /**
       Initialize field with ObjId and fieldName.
    */
    int moose_Field_init(_Field * self, PyObject * args, PyObject * kwargs)
    {
        PyObject * owner;
        char * fieldName;
        if (!PyArg_ParseTuple(args, "Os:moose_Field_init", &owner, &fieldName)){
            return -1;
        }
        if (fieldName == NULL){
            PyErr_SetString(PyExc_ValueError, "fieldName cannot be NULL");
            return -1;
        }
        if (owner == NULL){
            PyErr_SetString(PyExc_ValueError, "owner cannot be NULL");
            return -1;
        }
        if (!PyObject_IsInstance(owner, (PyObject*)&ObjIdType)){
            PyErr_SetString(PyExc_TypeError, "Owner must be subtype of ObjId");
            return -1;
        }
        self->owner = ((_ObjId*)owner)->oid_;
        if (!Id::isValid(self->owner.id)){
            Py_XDECREF(owner);
            Py_XDECREF(self);
            RAISE_INVALID_ID(-1, "moose_Field_init");
        }
        size_t size = strlen(fieldName);
        char * name = (char*)calloc(size+1, sizeof(char));
        strncpy(name, fieldName, size);
        self->name = name;
        // In earlier version I tried to deallocate the existing
        // self->name if it is not NULL. But it turns out that it
        // causes a SIGABRT. In any case it should not be an issue as
        // we can safely assume __init__ will be called only once in
        // this case. The Fields are created only internally at
        // initialization of the MOOSE module.
        return 0;
    }

    /// Return the hash of the string `{objectpath}.{fieldName}`
    long moose_Field_hash(_Field * self)
    {
        if (!Id::isValid(self->owner.id)){
            RAISE_INVALID_ID(-1, "moose_Field_hash");
        }
        string fieldPath = self->owner.path() + "." + self->name;
        PyObject * path = PyString_FromString(fieldPath.c_str());
        long hash = PyObject_Hash(path);
        Py_XDECREF(path);
        return hash;    
    }

    /// String representation of fields is `{objectpath}.{fieldName}`
    PyObject * moose_Field_repr(_Field * self)
    {
        if (!Id::isValid(self->owner.id)){
            RAISE_INVALID_ID(NULL, "moose_Field_repr");
        }
        ostringstream fieldPath;
        fieldPath << self->owner.path() << "." << self->name;
        return PyString_FromString(fieldPath.str().c_str());
    }

    
    PyDoc_STRVAR(moose_Field_documentation,
                 "Base class for MOOSE fields.\n"
                 "\n"
                 "Instances contain the field name and a pointer to the owner\n"
                 "object. Note on hash: the Field class is hashable but the hash is\n"
                 "constructed from the path of the container element and the field\n"
                 "name. Hence changing the name of the container element will cause the\n"
                 "hash to change. This is rather unusual in a moose script, but if you\n"
                 "are putting fields as dictionary keys, you should do that after names\n"
                 "of all elements have been finalized.");


    static PyTypeObject moose_Field = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "moose.Field",                                  /* tp_name */
        sizeof(_Field),                                 /* tp_basicsize */
        0,                                              /* tp_itemsize */
        0,// (destructor)moose_Field_dealloc,                /* tp_dealloc */
        0,                                              /* tp_print */
        0,                                              /* tp_getattr */
        0,                                              /* tp_setattr */
        0,                                              /* tp_compare */
        (reprfunc)moose_Field_repr,                     /* tp_repr */
        0,                                              /* tp_as_number */
        0,                                              /* tp_as_sequence */
        0,                                              /* tp_as_mapping */
        (hashfunc)moose_Field_hash,                     /* tp_hash */
        0,                                              /* tp_call */
        (reprfunc)moose_Field_repr,                     /* tp_str */
        0,                  /* tp_getattro */
        PyObject_GenericSetAttr,                        /* tp_setattro */
        0,                                              /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        moose_Field_documentation,
        0,                                              /* tp_traverse */
        0,                                              /* tp_clear */
        0,                                              /* tp_richcompare */
        0,                                              /* tp_weaklistoffset */
        0,                                              /* tp_iter */
        0,                                              /* tp_iternext */
        0,                                              /* tp_methods */
        0,                                              /* tp_members */
        0,                                              /* tp_getset */
        0,                                              /* tp_base */
        0,                                              /* tp_dict */
        0,                                              /* tp_descr_get */
        0,                                              /* tp_descr_set */
        0,                                              /* tp_dictoffset */
        (initproc)moose_Field_init,                     /* tp_init */
        0,                                              /* tp_alloc */
        0,                                              /* tp_new */
        0,                                              /* tp_free */
    };

    PyObject * moose_LookupField_getItem(_Field * self, PyObject * key)
    {
        return getLookupField(self->owner, self->name, key);
    }

    int moose_LookupField_setItem(_Field * self, PyObject * key,
                                         PyObject * value)
    {
        return setLookupField(self->owner,
                              self->name, key, value);
    }

    /**
       The mapping methods make it act like a Python dictionary.
    */
    static PyMappingMethods LookupFieldMappingMethods = {
        0,
        (binaryfunc)moose_LookupField_getItem,
        (objobjargproc)moose_LookupField_setItem,
    };

    PyDoc_STRVAR(moose_LookupField_documentation,
                 "LookupField is dictionary-like fields that map keys to values.\n"
                 "The keys need not be fixed, as in case of interpolation tables,\n"
                 "keys can be any number and the corresponding value is dynamically\n"
                 "computed by the method of interpolation.\n"
                 "Use moose.doc('classname.fieldname') to display builtin\n"
                 "documentation for `field` in class `classname`.\n");
    PyTypeObject moose_LookupField = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "moose.LookupField",                                  /* tp_name */
        sizeof(_Field),                                 /* tp_basicsize */
        0,                                              /* tp_itemsize */
        0,                /* tp_dealloc */
        0,                                              /* tp_print */
        0,                                              /* tp_getattr */
        0,                                              /* tp_setattr */
        0,                                              /* tp_compare */
        (reprfunc)moose_Field_repr,                     /* tp_repr */
        0,                                              /* tp_as_number */
        0,                                              /* tp_as_sequence */
        &LookupFieldMappingMethods,                      /* tp_as_mapping */
        (hashfunc)moose_Field_hash,                     /* tp_hash */
        0,                                              /* tp_call */
        (reprfunc)moose_Field_repr,                     /* tp_str */
        0,                  /* tp_getattro */
        PyObject_GenericSetAttr,                        /* tp_setattro */
        0,                                              /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,
        moose_LookupField_documentation,
        0,                                              /* tp_traverse */
        0,                                              /* tp_clear */
        0,                                              /* tp_richcompare */
        0,                                              /* tp_weaklistoffset */
        0,                                              /* tp_iter */
        0,                                              /* tp_iternext */
        0,                                              /* tp_methods */
        0,                                              /* tp_members */
        0,                                              /* tp_getset */
        &moose_Field,                                              /* tp_base */
        0,                                              /* tp_dict */
        0,                                              /* tp_descr_get */
        0,                                              /* tp_descr_set */
        0,                                              /* tp_dictoffset */
        (initproc)moose_Field_init,                     /* tp_init */
        0,                                              /* tp_alloc */
        0,                       /* tp_new */
        0,                                              /* tp_free */
    };


    PyObject * moose_DestField_call(PyObject * self, PyObject * args,
                                           PyObject * kw)
    {
        // We copy the name as the first argument into a new argument tuple. 
        PyObject * newargs = PyTuple_New(PyTuple_Size(args)+1); // one extra for the field name
        PyObject * name = PyString_FromString(((_Field*)self)->name);
        if (name == NULL){
            Py_XDECREF(newargs);
            return NULL;
        }
        if (PyTuple_SetItem(newargs, 0, name) != 0){
            Py_XDECREF(newargs);
            return NULL;
        }
        // We copy the arguments in `args` into the new argument tuple
        Py_ssize_t argc =  PyTuple_Size(args);
        for (Py_ssize_t ii = 0; ii < argc; ++ii){
            PyObject * arg = PyTuple_GetItem(args, ii);
            if (arg != NULL){
                PyTuple_SetItem(newargs, ii+1, arg);
            } else {
                Py_XDECREF(newargs);
                return NULL;
            }
        }
        // Call ObjId._setDestField with the new arguments
        _ObjId * obj = PyObject_New(_ObjId, &ObjIdType);
        obj->oid_ = ((_Field*)self)->owner;
        return moose_ObjId_setDestField(obj, newargs);
    }

    PyDoc_STRVAR(moose_DestField_documentation,
                 "DestField is a method field, i.e. it can be called like a function.\n"
                 "Use moose.doc('classname.fieldname') to display builtin\n"
                 "documentation for `field` in class `classname`.\n");
                 

    PyTypeObject moose_DestField = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "moose.DestField",                              /* tp_name */
        sizeof(_Field),                                 /* tp_basicsize */
        0,                                              /* tp_itemsize */
        0,                /* tp_dealloc */
        0,                                              /* tp_print */
        0,                                              /* tp_getattr */
        0,                                              /* tp_setattr */
        0,                                              /* tp_compare */
        (reprfunc)moose_Field_repr,                     /* tp_repr */
        0,                                              /* tp_as_number */
        0,                                              /* tp_as_sequence */
        0,                                              /* tp_as_mapping */
        (hashfunc)moose_Field_hash,                     /* tp_hash */
        moose_DestField_call,                           /* tp_call */
        (reprfunc)moose_Field_repr,                     /* tp_str */
        0,                                              /* tp_getattro */
        PyObject_GenericSetAttr,                        /* tp_setattro */
        0,                                              /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,
        moose_DestField_documentation,
        0,                                              /* tp_traverse */
        0,                                              /* tp_clear */
        0,                                              /* tp_richcompare */
        0,                                              /* tp_weaklistoffset */
        0,                                              /* tp_iter */
        0,                                              /* tp_iternext */
        0,                                              /* tp_methods */
        0,                                              /* tp_members */
        0,                                              /* tp_getset */
        &moose_Field,                                              /* tp_base */
        0,                                              /* tp_dict */
        0,                                              /* tp_descr_get */
        0,                                              /* tp_descr_set */
        0,                                              /* tp_dictoffset */
        (initproc)moose_Field_init,                     /* tp_init */
        0,                                              /* tp_alloc */
        0,                       /* tp_new */
        0,                                              /* tp_free */
    };

    PyDoc_STRVAR(moose_ElementField_documentation,
                 "ElementField represents fields that are themselves elements. For\n"
                 "example, synapse in an IntFire neuron. Element fields can be traversed\n"
                 "like a sequence. Additionally, you can set the number of entries by\n"
                 "setting the `num` attribute to a desired value.\n");

    PyDoc_STRVAR(moose_ElementField_num_documentation,
                 "Number of entries in the field.");

    PyDoc_STRVAR(moose_ElementField_path_documentation,
                 "Path of the field element.");
    PyDoc_STRVAR(moose_ElementField_id_documentation,
                 "Id of the field element.");
    PyDoc_STRVAR(moose_ElementField_dataId_documentation,
                 "dataIndex of the field element");
    /* These static defs are required for compiler complaining about string literals. */
    static char name[] = "name";
    static char numfield[] = "num";
    static char path[] = "path";
    static char id[] = "vec";
    static char owner[] = "owner";
    static char dataIndex[] = "dataIndex";
    static PyGetSetDef ElementFieldGetSetters[] = {
        {numfield,
         (getter)moose_ElementField_getNum,
         (setter)moose_ElementField_setNum,
         moose_ElementField_num_documentation,
         NULL},
        {path,
         (getter)moose_ElementField_getPath,
         NULL,
         moose_ElementField_path_documentation,
         NULL},
        {id,
         (getter)moose_ElementField_getId,
         NULL,
         moose_ElementField_id_documentation,
         NULL},
        {name,
         (getter)moose_ElementField_getName,
         NULL,
         (char*)"",
         NULL},
        {owner,
         (getter)moose_ElementField_getOwner,
         NULL,
         (char*)"",
         NULL},
        {dataIndex,
         (getter)moose_ElementField_getDataId,
         NULL,
         moose_ElementField_dataId_documentation,
         NULL},
        {NULL}, /* sentinel */
    };
    
    static PySequenceMethods ElementFieldSequenceMethods = {
        (lenfunc)moose_ElementField_getLen, // sq_length
        0, //sq_concat
        0, //sq_repeat
        (ssizeargfunc)moose_ElementField_getItem, //sq_item
#ifndef PY3K
        (ssizessizeargfunc)moose_ElementField_getSlice, // getslice
#endif
        0, //sq_ass_item
#ifndef PY3K
        0, // setslice
#endif
        0, // sq_contains
        0, // sq_inplace_concat
        0 // sq_inplace_repeat
    };

    PyTypeObject moose_ElementField = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "moose.ElementField",                              /* tp_name */
        sizeof(_Field),                                 /* tp_basicsize */
        0,                                              /* tp_itemsize */
        0,                                              /* tp_dealloc */
        0,                                              /* tp_print */
        0,                                              /* tp_getattr */
        0,                                              /* tp_setattr */
        0,                                              /* tp_compare */
        (reprfunc)moose_Field_repr,                     /* tp_repr */
        0,                                              /* tp_as_number */
        &ElementFieldSequenceMethods,                   /* tp_as_sequence */
        0,                                              /* tp_as_mapping */
        (hashfunc)moose_Field_hash,                     /* tp_hash */
        0,                                              /* tp_call */
        (reprfunc)moose_Field_repr,                     /* tp_str */
        (getattrofunc)moose_ElementField_getattro,      /* tp_getattro */
        (setattrofunc)moose_ElementField_setattro,      /* tp_setattro */
        0,                                              /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,
        moose_ElementField_documentation,
        0,                                              /* tp_traverse */
        0,                                              /* tp_clear */
        0, // (richcmpfunc)moose_ElementField_richcmp,        /* tp_richcompare */
        0,                                              /* tp_weaklistoffset */
        0,                                              /* tp_iter */
        0,                                              /* tp_iternext */
        0,                            /* tp_methods */
        0,                            /* tp_members */
        ElementFieldGetSetters,                         /* tp_getset */
        &moose_Field,                                   /* tp_base */
        0,                                              /* tp_dict */
        0,                                              /* tp_descr_get */
        0,                                              /* tp_descr_set */
        0,                                              /* tp_dictoffset */
        (initproc)moose_ElementField_init,                     /* tp_init */
        0,                                              /* tp_alloc */
        0,                       /* tp_new */
        0,                                              /* tp_free */        
    };

    /**
       Initialize field with ObjId and fieldName.
    */
    int moose_ElementField_init(_Field * self, PyObject * args, PyObject * kwargs)
    {
        moose_Field_init(self, args, kwargs);
        string path = self->owner.path()+"/";
        path += string(self->name);
        self->myoid = ObjId(path);
        return 0;
    }

    PyObject * moose_ElementField_getNum(_Field * self, void * closure)
    {
        if (self->owner.bad()){
            RAISE_INVALID_ID(NULL, "moose_ElementField_getNum");
        }
        unsigned int num = Field<unsigned int>::get(self->owner, "num_" + string(self->name));
        return Py_BuildValue("I", num);
    }

    int moose_ElementField_setNum(_Field * self, PyObject * args, void * closure)
    {
        if (self->owner.bad()){
            RAISE_INVALID_ID(-1, "moose_ElementField_setNum");
        }
        unsigned int num;
        if (!PyInt_Check(args)){
            PyErr_SetString(PyExc_TypeError, "moose.ElementField.setNum - needes an integer.");
            return -1;
        }
        num = PyInt_AsUnsignedLongMask(args);
        if (!Field<unsigned int>::set(self->myoid, "numField", num)){
            PyErr_SetString(PyExc_RuntimeError, "moose.ElementField.setNum : Field::set returned False.");
            return -1;
        }
        return 0;
    }

    Py_ssize_t moose_ElementField_getLen(_Field * self, void * closure)
    {
        if (self->owner.bad()){
            RAISE_INVALID_ID(-1, "moose_ElementField_getLen");
        }
        unsigned int num = Field<unsigned int>::get(self->myoid, "numField");
        return Py_ssize_t(num);
    }



    PyObject * moose_ElementField_getPath(_Field * self, void * closure)
    {
        if (!Id::isValid(self->owner.id)){
            RAISE_INVALID_ID(NULL, "moose_ElementField_setNum");
        }
        string path = Id(self->owner.path() + "/" + string(self->name)).path();
        return Py_BuildValue("s", path.c_str());
    }

    PyObject * moose_ElementField_getId(_Field * self, void * closure)
    {
        if (self->owner.bad()){
            RAISE_INVALID_ID(NULL, "moose_ElementField_setNum");
        }
        Id myId(self->owner.path() + "/" + string(self->name));
        _Id * new_id = PyObject_New(_Id, &IdType);
        new_id->id_ = myId;
        return (PyObject*)new_id;
    }

    PyObject * moose_ElementField_getName(_Field * self, void * closure)
    {
        return Py_BuildValue("s", self->name);
    }
    PyObject * moose_ElementField_getOwner(_Field * self, void * closure)
    {
        _ObjId * owner = PyObject_New(_ObjId, &ObjIdType);
        owner->oid_ = self->owner;
        return (PyObject*)owner;
    }
    PyObject * moose_ElementField_getDataId(_Field * self, void * closure)
    {
        if (self->owner.bad()){
            RAISE_INVALID_ID(NULL, "moose_ElementField_getItem");
        }
        return Py_BuildValue("I", self->owner.dataIndex);
    }

    PyObject * moose_ElementField_getItem(_Field * self, Py_ssize_t index)
    {
        if (self->owner.bad()){
            RAISE_INVALID_ID(NULL, "moose_ElementField_getItem");
        }
        unsigned int len = Field<unsigned int>::get(self->myoid, "numField");
        if (index >= len){
            PyErr_SetString(PyExc_IndexError, "moose.ElementField.getItem: index out of bounds.");
            return NULL;
        }
        if (index < 0){
            index += len;
        }
        if (index < 0){
            PyErr_SetString(PyExc_IndexError, "moose.ElementField.getItem: invalid index.");
            return NULL;
        }
        // _ObjId * oid = PyObject_New(_ObjId, &ObjIdType);
        // cout << "Element field: " << self->name << ", owner: " << self->owner.path() << endl;
        // stringstream path;
        // path << self->owner.path() << "/" << self->name << "[" << index << "]";
        // cout << "moose_ElementField_getItem:: path=" << path.str();
        // oid->oid_ = ObjId(self->myoid.id, self->myoid.dataIndex, index);
        // return (PyObject*)oid;
        ObjId oid(self->myoid.id, self->myoid.dataIndex, index);
        return oid_to_element(oid);
    }
    
    PyObject * moose_ElementField_getSlice(_Field * self, Py_ssize_t start, Py_ssize_t end)
    {
        if (self->owner.bad()){
            RAISE_INVALID_ID(NULL, "moose_ElementField_getSlice");
        }        
        extern PyTypeObject ObjIdType;
        Py_ssize_t len = Field<unsigned int>::get(self->myoid, "numField");
        while (start < 0){
            start += len;
        }
        while (end < 0){
            end += len;
        }
        if (start > end){
            // PyErr_SetString(PyExc_IndexError, "Start index must be less than end.");
            // return NULL;
            // Python itself returns empty tuple in such cases, follow that
            return PyTuple_New(0);
        }
        PyObject * ret = PyTuple_New((Py_ssize_t)(end - start));
        
        // Py_XINCREF(ret);        
        for (unsigned int ii = start; ii < end; ++ii){
            ObjId oid(self->myoid.id, self->myoid.dataIndex, ii);
            PyObject * value = oid_to_element(oid);
            if (PyTuple_SetItem(ret, (Py_ssize_t)(ii-start), value)){
                Py_XDECREF(ret);
                Py_XDECREF(value);
                PyErr_SetString(PyExc_RuntimeError, "Could assign tuple entry.");
                return NULL;
            }
        }
        return ret;
    }

    PyObject * moose_ElementField_getattro(_Field * self, PyObject * attr)
    {
        if (self->owner.bad()){
            RAISE_INVALID_ID(NULL, "moose_ElementField_getSlice");
        }
        char * field = PyString_AsString(attr);
        string className = Field<string>::get(self->myoid, "className");
        string type = getFieldType(className, field);
        if (type.empty()){
            // Check if this field name is aliased and update fieldname and type if so.
            map<string, string>::const_iterator it = get_field_alias().find(string(field));
            if (it != get_field_alias().end()){
                field = const_cast<char*>((it->second).c_str());
                type = getFieldType(Field<string>::get(self->myoid, "className"), it->second);
                // Update attr for next level (PyObject_GenericGetAttr) in case.
                Py_XDECREF(attr);
                attr = PyString_FromString(field);
            }
        }
        if (type.empty()){
            return PyObject_GenericGetAttr((PyObject*)self, attr);            
        }
        char ftype = shortType(type);
        if (!ftype){
            return PyObject_GenericGetAttr((PyObject*)self, attr);
        }

        switch (ftype){
            case 'd': {
                vector < double > val;
                Field< double >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 's': {
                vector < string > val;
                Field< string >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 'l': {
                vector < long > val;
                Field< long >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 'x': {
                vector < Id > val;
                Field< Id >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 'y': {
                vector < ObjId > val;
                Field< ObjId >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 'i': {
                vector < int > val;
                Field< int >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 'I': {
                vector < unsigned int > val;
                Field< unsigned int >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 'k': {
                vector < unsigned long > val;
                Field< unsigned long >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 'f': {
                vector < float > val;
                Field< float >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }            
            case 'b': {                                                               
                vector<bool> val;
                Field< bool >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 'c': {
                vector < char > val;
                Field< char >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 'h': {
                vector < short > val;
                Field< short >::getVec(self->myoid, string(field), val);
                return to_pytuple(&val, ftype);
            }
            case 'z': {
                PyErr_SetString(PyExc_NotImplementedError, "DataId handling not implemented yet.");
                return NULL;
            }
            default:
                PyErr_SetString(PyExc_ValueError, "unhandled field type.");
                return NULL;                
        }
        
    }
    
    int moose_ElementField_setattro(_Field * self, PyObject * attr, PyObject * value)
    {
        if (!Id::isValid(self->myoid)){
            RAISE_INVALID_ID(-1, "moose_ElementField_setattro");
        }
        int ret = -1;
        string field;
        if (PyString_Check(attr)){
            field = string(PyString_AsString(attr));
        } else {
            PyErr_SetString(PyExc_TypeError, "Attribute name must be a string");
            return -1;
        }
        string moose_class = Field<string>::get(self->myoid, "className");
        string fieldtype = getFieldType(moose_class, field);
        if (fieldtype.length() == 0){
            if (field == "num"){
                return PyObject_GenericSetAttr((PyObject*)self, attr, value);
            }
            PyErr_SetString(PyExc_AttributeError, "cannot add new field to ElementField objects");
            return -1;
        }
        char ftype = shortType(fieldtype);
        Py_ssize_t length = moose_ElementField_getLen(self, NULL);
        bool is_seq = true;
        if (!PySequence_Check(value)){
            is_seq = false;
        } else if (length != PySequence_Length(value)){
            PyErr_SetString(PyExc_IndexError, "Length of the sequence on the right hand side does not match Id size.");
            return -1;
        }
        switch(ftype){
            case 'd': {//SET_VECFIELD(double, d)
                vector<double> _value;
                if (is_seq){
                    for (unsigned int ii = 0; ii < length; ++ii){
                        double v = PyFloat_AsDouble(PySequence_GetItem(value, ii));
                        _value.push_back(v);
                    }
                } else {
                    double v = PyFloat_AsDouble(value);
                    _value.assign(length, v);
                }
                ret = Field<double>::setVec(self->myoid, field, _value);
                break;
            }                
            case 's': {
                vector<string> _value;
                if (is_seq){
                    for (unsigned int ii = 0; ii < length; ++ii){
                        char * v = PyString_AsString(PySequence_GetItem(value, ii));
                        _value.push_back(string(v));
                    }
                } else {
                    char * v = PyString_AsString(value);
                    _value.assign(length, string(v));
                }                    
                ret = Field<string>::setVec(self->myoid, field, _value);
                break;
            }
            case 'i': {
                vector<int> _value;
                if (is_seq){
                    for (unsigned int ii = 0; ii < length; ++ii){
                        int v = PyInt_AsLong(PySequence_GetItem(value, ii));
                        _value.push_back(v);
                    }
                } else {
                    int v = PyInt_AsLong(value);
                    _value.assign(length, v);
                }
                ret = Field< int >::setVec(self->myoid, field, _value);
                break;
            }
            case 'I': {//SET_VECFIELD(unsigned int, I)
                vector<unsigned int> _value;
                if (is_seq){
                    for (unsigned int ii = 0; ii < length; ++ii){
                        unsigned int v = PyInt_AsUnsignedLongMask(PySequence_GetItem(value, ii));
                        _value.push_back(v);
                    }
                } else {
                    unsigned int v = PyInt_AsUnsignedLongMask(value);
                    _value.assign(length, v);
                }
                ret = Field< unsigned int >::setVec(self->myoid, field, _value);                
                break;
            }
            case 'l': {//SET_VECFIELD(long, l)
                vector<long> _value;
                if (is_seq){
                    for (unsigned int ii = 0; ii < length; ++ii){
                        long v = PyInt_AsLong(PySequence_GetItem(value, ii));
                        _value.push_back(v);
                    }
                } else {
                    long v = PyInt_AsLong(value);
                    _value.assign(length, v);                    
                }
                ret = Field<long>::setVec(self->myoid, field, _value);
                break;
            }
            case 'k': {//SET_VECFIELD(unsigned long, k)
                vector<unsigned long> _value;
                if (is_seq){
                    for (unsigned int ii = 0; ii < length; ++ii){
                        unsigned long v = PyInt_AsUnsignedLongMask(PySequence_GetItem(value, ii));
                        _value.push_back(v);
                    }
                } else {
                    unsigned long v = PyInt_AsUnsignedLongMask(value);
                    _value.assign(length, v);
                }
                ret = Field< unsigned long >::setVec(self->myoid, field, _value);                
                break;
            }
            case 'b': {
                vector<bool> _value;
                if (is_seq){
                    for (unsigned int ii = 0; ii < length; ++ii){
                        PyObject * _v = PySequence_GetItem(value, ii);
                        bool v = (Py_True ==_v) || (PyInt_AsLong(_v) != 0);
                        _value.push_back(v);
                    }
                } else {
                    bool v = (Py_True ==value) || (PyInt_AsLong(value) != 0);
                    _value.assign(length, v);
                }
                ret = Field< bool >::setVec(self->myoid, field, _value);
                break;
            }
            case 'c': {
                vector<char> _value;
                if (is_seq){
                    for (unsigned int ii = 0; ii < length; ++ii){
                        PyObject * _v = PySequence_GetItem(value, ii);
                        char * v = PyString_AsString(_v);
                        if (v && v[0]){
                            _value.push_back(v[0]);
                        } else {
                            ostringstream err;
                            err << ii << "-th element is NUL";
                            PyErr_SetString(PyExc_ValueError, err.str().c_str());
                            return -1;
                        }
                    }
                } else {
                    char * v = PyString_AsString(value);
                    if (v && v[0]){
                        _value.assign(length, v[0]);
                    } else {
                        PyErr_SetString(PyExc_ValueError,  "value is an empty string");
                        return -1;
                    }
                }                    
                ret = Field< char >::setVec(self->myoid, field, _value);
                break;
            }
            case 'h': {
                vector<short> _value;
                if (is_seq){
                    for (unsigned int ii = 0; ii < length; ++ii){
                        short v = PyInt_AsLong(PySequence_GetItem(value, ii));
                        _value.push_back(v);
                    }
                } else {
                    short v = PyInt_AsLong(value);
                    _value.assign(length, v);
                }
                ret = Field< short >::setVec(self->myoid, field, _value);
                break;
            }
            case 'f': {//SET_VECFIELD(float, f)
                vector<float> _value;
                if (is_seq){
                    for (unsigned int ii = 0; ii < length; ++ii){
                        float v = PyFloat_AsDouble(PySequence_GetItem(value, ii));
                        _value.push_back(v);
                    }
                } else {
                    float v = PyFloat_AsDouble(value);                    
                    _value.assign(length, v);
                }
                ret = Field<float>::setVec(self->myoid, field, _value);
                break;
            }
            default:                
                break;
        }
        // MOOSE Field::set returns 1 for success 0 for
        // failure. Python treats return value 0 from setters as
        // success, anything else failure.
        if (ret || (PyErr_Occurred() == NULL)){
            return 0;
        } else {
            return -1;
        }
        
    }
            
    // PyObject * moose_ElementField_richcmp(_Field * self, void * closure);

} // extern "C"


// 
// mfield.cpp ends here
