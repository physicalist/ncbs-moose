/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.13.2 on Mon Dec 29 13:39:03 2014
 */

#include "sipAPI_moogli.h"

#line 8 "sip/_moogli.sip"
#include "core/SelectInfo.hpp"
#line 12 "build/temp.linux-x86_64-2.7/sip_moogliSelectInfo.cpp"



extern "C" {static PyObject *meth_SelectInfo_get_id(PyObject *, PyObject *);}
static PyObject *meth_SelectInfo_get_id(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        SelectInfo *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "B", &sipSelf, sipType_SelectInfo, &sipCpp))
        {
            const char *sipRes;

            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipRes = sipCpp->get_id();
            }
            catch (...)
            {
                Py_BLOCK_THREADS

                sipRaiseUnknownException();
                return NULL;
            }
            Py_END_ALLOW_THREADS

            if (sipRes == NULL)
            {
                Py_INCREF(Py_None);
                return Py_None;
            }

            return SIPBytes_FromString(sipRes);
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_SelectInfo, sipName_get_id, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_SelectInfo_get_event_type(PyObject *, PyObject *);}
static PyObject *meth_SelectInfo_get_event_type(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        SelectInfo *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "B", &sipSelf, sipType_SelectInfo, &sipCpp))
        {
            int sipRes;

            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipRes = sipCpp->get_event_type();
            }
            catch (...)
            {
                Py_BLOCK_THREADS

                sipRaiseUnknownException();
                return NULL;
            }
            Py_END_ALLOW_THREADS

            return SIPLong_FromLong(sipRes);
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_SelectInfo, sipName_get_event_type, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_SelectInfo_set_id(PyObject *, PyObject *);}
static PyObject *meth_SelectInfo_set_id(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        const char * a0;
        SelectInfo *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "Bs", &sipSelf, sipType_SelectInfo, &sipCpp, &a0))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp->set_id(a0);
            }
            catch (...)
            {
                Py_BLOCK_THREADS

                sipRaiseUnknownException();
                return NULL;
            }
            Py_END_ALLOW_THREADS

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_SelectInfo, sipName_set_id, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_SelectInfo_set_event_type(PyObject *, PyObject *);}
static PyObject *meth_SelectInfo_set_event_type(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        int a0;
        SelectInfo *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "Bi", &sipSelf, sipType_SelectInfo, &sipCpp, &a0))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp->set_event_type(a0);
            }
            catch (...)
            {
                Py_BLOCK_THREADS

                sipRaiseUnknownException();
                return NULL;
            }
            Py_END_ALLOW_THREADS

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_SelectInfo, sipName_set_event_type, NULL);

    return NULL;
}


/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_SelectInfo(void *, const sipTypeDef *);}
static void *cast_SelectInfo(void *ptr, const sipTypeDef *targetType)
{
    if (targetType == sipType_SelectInfo)
        return ptr;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_SelectInfo(void *, int);}
static void release_SelectInfo(void *sipCppV,int)
{
    Py_BEGIN_ALLOW_THREADS

    delete reinterpret_cast<SelectInfo *>(sipCppV);

    Py_END_ALLOW_THREADS
}


extern "C" {static void assign_SelectInfo(void *, SIP_SSIZE_T, const void *);}
static void assign_SelectInfo(void *sipDst, SIP_SSIZE_T sipDstIdx, const void *sipSrc)
{
    reinterpret_cast<SelectInfo *>(sipDst)[sipDstIdx] = *reinterpret_cast<const SelectInfo *>(sipSrc);
}


extern "C" {static void *array_SelectInfo(SIP_SSIZE_T);}
static void *array_SelectInfo(SIP_SSIZE_T sipNrElem)
{
    return new SelectInfo[sipNrElem];
}


extern "C" {static void *copy_SelectInfo(const void *, SIP_SSIZE_T);}
static void *copy_SelectInfo(const void *sipSrc, SIP_SSIZE_T sipSrcIdx)
{
    return new SelectInfo(reinterpret_cast<const SelectInfo *>(sipSrc)[sipSrcIdx]);
}


extern "C" {static void dealloc_SelectInfo(sipSimpleWrapper *);}
static void dealloc_SelectInfo(sipSimpleWrapper *sipSelf)
{
    if (sipIsPyOwned(sipSelf))
    {
        release_SelectInfo(sipGetAddress(sipSelf),0);
    }
}


extern "C" {static void *init_SelectInfo(sipSimpleWrapper *, PyObject *, PyObject *, PyObject **, PyObject **, PyObject **);}
static void *init_SelectInfo(sipSimpleWrapper *, PyObject *sipArgs, PyObject *sipKwds, PyObject **sipUnused, PyObject **, PyObject **sipParseErr)
{
    SelectInfo *sipCpp = 0;

    {
        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, ""))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp = new SelectInfo();
            }
            catch (...)
            {
                Py_BLOCK_THREADS

                sipRaiseUnknownException();
                return NULL;
            }
            Py_END_ALLOW_THREADS

            return sipCpp;
        }
    }

    {
        const SelectInfo * a0;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "J9", sipType_SelectInfo, &a0))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp = new SelectInfo(*a0);
            }
            catch (...)
            {
                Py_BLOCK_THREADS

                sipRaiseUnknownException();
                return NULL;
            }
            Py_END_ALLOW_THREADS

            return sipCpp;
        }
    }

    return NULL;
}


static PyMethodDef methods_SelectInfo[] = {
    {SIP_MLNAME_CAST(sipName_get_event_type), meth_SelectInfo_get_event_type, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_get_id), meth_SelectInfo_get_id, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_set_event_type), meth_SelectInfo_set_event_type, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_set_id), meth_SelectInfo_set_id, METH_VARARGS, NULL}
};


pyqt4ClassTypeDef sipTypeDef__moogli_SelectInfo = {
{
    {
        -1,
        0,
        0,
        SIP_TYPE_CLASS,
        sipNameNr_SelectInfo,
        {0}
    },
    {
        sipNameNr_SelectInfo,
        {0, 0, 1},
        4, methods_SelectInfo,
        0, 0,
        0, 0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    0,
    sipNameNr_PyQt4_QtCore_pyqtWrapperType,
    -1,
    0,
    0,
    init_SelectInfo,
    0,
    0,
#if PY_MAJOR_VERSION >= 3
    0,
    0,
#else
    0,
    0,
    0,
    0,
#endif
    dealloc_SelectInfo,
    assign_SelectInfo,
    array_SelectInfo,
    copy_SelectInfo,
    release_SelectInfo,
    cast_SelectInfo,
    0,
    0,
    0
},
    0,
    0,
    0
};
