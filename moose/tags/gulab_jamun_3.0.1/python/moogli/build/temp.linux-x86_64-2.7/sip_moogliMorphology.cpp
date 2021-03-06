/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.13.2 on Mon Dec 29 13:39:03 2014
 */

#include "sipAPI_moogli.h"

#line 22 "sip/_moogli.sip"
#include "includes.hpp"
#include "core/Compartment.hpp"
#include "core/Morphology.hpp"
#line 14 "build/temp.linux-x86_64-2.7/sip_moogliMorphology.cpp"



extern "C" {static PyObject *meth_Morphology_add_compartment(PyObject *, PyObject *);}
static PyObject *meth_Morphology_add_compartment(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        const char * a0;
        const char * a1;
        double a2;
        double a3;
        double a4;
        double a5;
        double a6;
        double a7;
        double a8;
        double a9;
        Morphology *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "Bssdddddddd", &sipSelf, sipType_Morphology, &sipCpp, &a0, &a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9))
        {
            int sipRes;

            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipRes = sipCpp->add_compartment(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9);
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
    sipNoMethod(sipParseErr, sipName_Morphology, sipName_add_compartment, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_Morphology_set_compartment_order(PyObject *, PyObject *);}
static PyObject *meth_Morphology_set_compartment_order(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        PyObject * a0;
        Morphology *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "BP0", &sipSelf, sipType_Morphology, &sipCpp, &a0))
        {
            PyObject * sipRes;

            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipRes = sipCpp->set_compartment_order(a0);
            }
            catch (...)
            {
                Py_BLOCK_THREADS

                sipRaiseUnknownException();
                return NULL;
            }
            Py_END_ALLOW_THREADS

            return sipRes;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_Morphology, sipName_set_compartment_order, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_Morphology_set_initial_color(PyObject *, PyObject *);}
static PyObject *meth_Morphology_set_initial_color(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        float a0;
        float a1;
        float a2;
        float a3;
        Morphology *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "Bffff", &sipSelf, sipType_Morphology, &sipCpp, &a0, &a1, &a2, &a3))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp->set_initial_color(a0,a1,a2,a3);
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
    sipNoMethod(sipParseErr, sipName_Morphology, sipName_set_initial_color, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_Morphology_set_final_color(PyObject *, PyObject *);}
static PyObject *meth_Morphology_set_final_color(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        float a0;
        float a1;
        float a2;
        float a3;
        Morphology *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "Bffff", &sipSelf, sipType_Morphology, &sipCpp, &a0, &a1, &a2, &a3))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp->set_final_color(a0,a1,a2,a3);
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
    sipNoMethod(sipParseErr, sipName_Morphology, sipName_set_final_color, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_Morphology_set_membrane_voltages(PyObject *, PyObject *);}
static PyObject *meth_Morphology_set_membrane_voltages(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        PyObject * a0;
        Morphology *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "BP0", &sipSelf, sipType_Morphology, &sipCpp, &a0))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp->set_membrane_voltages(a0);
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
    sipNoMethod(sipParseErr, sipName_Morphology, sipName_set_membrane_voltages, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_Morphology_set_base_membrane_voltage(PyObject *, PyObject *);}
static PyObject *meth_Morphology_set_base_membrane_voltage(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        double a0;
        Morphology *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "Bd", &sipSelf, sipType_Morphology, &sipCpp, &a0))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp->set_base_membrane_voltage(a0);
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
    sipNoMethod(sipParseErr, sipName_Morphology, sipName_set_base_membrane_voltage, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_Morphology_set_peak_membrane_voltage(PyObject *, PyObject *);}
static PyObject *meth_Morphology_set_peak_membrane_voltage(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        double a0;
        Morphology *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "Bd", &sipSelf, sipType_Morphology, &sipCpp, &a0))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp->set_peak_membrane_voltage(a0);
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
    sipNoMethod(sipParseErr, sipName_Morphology, sipName_set_peak_membrane_voltage, NULL);

    return NULL;
}


/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_Morphology(void *, const sipTypeDef *);}
static void *cast_Morphology(void *ptr, const sipTypeDef *targetType)
{
    if (targetType == sipType_Morphology)
        return ptr;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_Morphology(void *, int);}
static void release_Morphology(void *sipCppV,int)
{
    Py_BEGIN_ALLOW_THREADS

    delete reinterpret_cast<Morphology *>(sipCppV);

    Py_END_ALLOW_THREADS
}


extern "C" {static void dealloc_Morphology(sipSimpleWrapper *);}
static void dealloc_Morphology(sipSimpleWrapper *sipSelf)
{
    if (sipIsPyOwned(sipSelf))
    {
        release_Morphology(sipGetAddress(sipSelf),0);
    }
}


extern "C" {static void *init_Morphology(sipSimpleWrapper *, PyObject *, PyObject *, PyObject **, PyObject **, PyObject **);}
static void *init_Morphology(sipSimpleWrapper *, PyObject *sipArgs, PyObject *sipKwds, PyObject **sipUnused, PyObject **, PyObject **sipParseErr)
{
    Morphology *sipCpp = 0;

    {
        const char * a0 = "";
        uint a1 = 3;
        float a2 = 50;
        uint a3 = 10;
        uint a4 = 2;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "|sufuu", &a0, &a1, &a2, &a3, &a4))
        {
            Py_BEGIN_ALLOW_THREADS
            try
            {
            sipCpp = new Morphology(a0,a1,a2,a3,a4);
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


static PyMethodDef methods_Morphology[] = {
    {SIP_MLNAME_CAST(sipName_add_compartment), meth_Morphology_add_compartment, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_set_base_membrane_voltage), meth_Morphology_set_base_membrane_voltage, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_set_compartment_order), meth_Morphology_set_compartment_order, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_set_final_color), meth_Morphology_set_final_color, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_set_initial_color), meth_Morphology_set_initial_color, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_set_membrane_voltages), meth_Morphology_set_membrane_voltages, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_set_peak_membrane_voltage), meth_Morphology_set_peak_membrane_voltage, METH_VARARGS, NULL}
};


pyqt4ClassTypeDef sipTypeDef__moogli_Morphology = {
{
    {
        -1,
        0,
        0,
        SIP_TYPE_CLASS,
        sipNameNr_Morphology,
        {0}
    },
    {
        sipNameNr_Morphology,
        {0, 0, 1},
        7, methods_Morphology,
        0, 0,
        0, 0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    0,
    sipNameNr_PyQt4_QtCore_pyqtWrapperType,
    -1,
    0,
    0,
    init_Morphology,
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
    dealloc_Morphology,
    0,
    0,
    0,
    release_Morphology,
    cast_Morphology,
    0,
    0,
    0
},
    0,
    0,
    0
};
