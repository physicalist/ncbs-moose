// pymoose.cpp --- 
// 
// Filename: pymoose.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Fri Mar 11 09:50:26 2011 (+0530)
// Version: 
// Last-Updated: Tue Mar 22 17:32:08 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 192
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
// 2011-03-11 09:50:30 (+0530) Splitting C++ wrappers from C module.
// 
// 

// Code:

#include "pymoose.h"

#include <typeinfo>
#include "moosemodule.h"
#include "../basecode/header.h"
#include "../basecode/ObjId.h"
#include "../basecode/DataId.h"

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



PyMooseBase::PyMooseBase(){}

PyMooseBase::~PyMooseBase(){}

pymoose_Neutral::pymoose_Neutral(Id id): id_(id)
{
}

pymoose_Neutral::pymoose_Neutral(string path, string type, vector<unsigned int> dims)
{
    id_ = Id(path);
    // If object does not exist, create new
    if ((id_ == Id()) && (path != "/") && (path != "/root")){
        string parent_path;
        if (path[0] != "/"){
            parent_path = getShell().getCwe().path();
        }
        size_t pos = path.rfind("/");
        string name, parent_path;
        if (pos != string::npos){
            name = path.substr(pos+1);
            parent_path += "/";
            parent_path += path.substr(0, pos+1);
        }
        id_ = getShell().doCreate(type, Id(parent_path), name, dims);
    }
}

pymoose_Neutral::~pymoose_Neutral()
{
}

int pymoose_Neutral::destroy()
{
    id_.destroy();
    return 1;
}
Id pymoose_Neutral::id()
{
    return id_;
}

void * pymoose_Neutral::getField(unsigned int index, string fieldName, char ftype)
{
    switch(ftype){
        case 'c':
            {
                char * ret = new char();
                *ret = Field< char >::get(ObjId(id_, index), fname);
                return (void*)ret;
            }
        case 'i':
            {
                long* ret = new long();
                * ret = Field< int >::get(ObjId(id_, index), fname);
                return (void*)ret;
            }
        case 'f':
            {
                double * ret = new double();
                *ret = Field< double >::get(ObjId(id_, index), fname);
                return (void*) ret;
            }
        case 's':
            {
                string val = Field< string >::get(ObjId(id_, index), fname);
                string * ret = new string(val);
                return (void*)ret;
            }
            break;
        case 'u':
            {
                unsigned long * ret = new unsigned long();
                *ret = Field<unsigned int>::get(ObjId(id_, index), fname);
                return (void*)ret;
            }
            break;
        case 'v':
            {
                vector<int>* ret = new vector<int>();
                *ret = Field< vector<int> >::get(ObjId((*(instance->id_)), index), string(fname));
                return (void*)ret;
            }
        case 'w':
            {
                vector<double>* ret = new vector<double>();
                *ret = Field< vector<double> >::get(ObjId(id_, index), string(fname));
                return (void*)ret;
            }
        case 'y': // element type - Ids
            {
                vector <Id>* ret = new vector<Id>();
                *ret = Field< vector<Id> >::get(ObjId(id_, index), "children");
                return (void*) ret;
            }
        default:
            return NULL;
    }     
}

int pymoose_Neutral::setField(unsigned int index, string fname, char ftype, void * value)
{
    switch(ftype){
        case 'c':
            {
                char  c_val = *((char*)value);
                Field<char>::set(ObjId(id_, index), fname, c_val);
                break;
            }                
        case 'i':
            {
                int c_val = *((int*)value);
                Field<int>::set(ObjId(id_, index), fname, c_val);
                break;
            }
        case 'f':
            {
                double c_val = *((double*)value);
                Field<double>::set(ObjId(id_, index), fname, c_val);
                break;
            }
        case 's':
            {
                char * c_val = *((char**)value);
                Field< string >::set(ObjId(id_, index), string(fname), string(c_val));
                break;
            }
        default:
            return 0;            
    };
    return 1;    
}
/**
   Return a list containing the names of fields for a specified finfo
   type.

   ftypeType can be one of: srcFinfo, destFinfo, valueFinfo,
   lookupFinfo, sharedFinfo.
 */
vector<string> pymoose_Neutral::getFieldNames(string ftypeType)
{
    string className = Field<string>::get(ObjeId(id_, 0), "class");
    string classInfoPath("/classes/" + className);
    Id classId(classInfoPath);
    unsigned int numFinfos = Field<unsigned int>::get(ObjId(classId,0), "num_" + ftypeType);
#ifndef NDEBUG
    cout << "vector<string> pymoose_Neutral::getFieldNames(string ftypeType): "
         << "class: " << classInfoPath
         << "numFinfos of type: " << ftypeType << " = " << numFinfos << endl;
#endif // !NDEBUG
    Id fieldId(classInfoPath + "/" + ftypeType);
    vector<string> fields;
    if (fieldId() != Id()){
        for (unsigned int ii = 0; ii < numFinfos; ++ii){
            string fieldName = Field<string>::get(ObjId(fieldId, ii), "name");
#ifndef NDEBUG       
            cout << "vector<string> pymoose_Neutral::getFieldNames(string ftypeType): "
                 << "field[" << ii << "] = " << fieldName << endl;
#endif // !NDEBUG
            fields.push_back(fieldName);
        } // ! for (unsigned int ii = 0; ii < numFinfos; ++ii){
    } // ! if (fieldId() != Id()){
    return fields;
}
string pymoose_Neutral::getFieldType(string fieldName)
{
    string fieldType;
    string className = Field<string>::get(ObjeId(id_, 0), "class");
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

vector<Id> pymoose_Neutral::getChildren(unsigned int index=0)
{
    vector<Id> children = Field< vector<Id> >::get(ObjId(id_, DataId(0,index)), "children");
    return children;
}

const map<string, string>& getArgMap()
{
    static map<string, string> argmap;
    if (argmap.empty()){
        string isSingleThreaded(getenv("SINGLETHREADED"));
        string isInfinite(getenv("INFINITE"));
        string numCores(getenv("NUMCORES"));
        string numNodes(getenv("NUMNODES"));
        argmap.insert(pair<string, string>("SINGLETHREADED", isSingleThreaded));
        argmap.insert(pair<string, string>("INFINITE", isInfinite));
        argmap.insert(pair<string, string>("NUMNODES", numNodes));
        argmap.insert(pair<string, string>("NUMCORES", numCores));        
    }
    return argmap;
}

const Shell& getShell()
{
    static Shell* shell = NULL;
    if (shell){
        return *shell;
    }
    // Set up the system parameters
    long isSingleThreaded = 0;
    long numCores = 1;
    long numNodes = 1;
    long isInfinite = 0;
    long myNode = 0;
    string arg;

    arg = getArgMap()["SINGLETHREADED"];    
    if (!arg.empty()){
        istringstream(arg) >> isSingleThreaded;
    }
    arg = getArgMap()["NUMCORES"];
    if (arg.empty()){
        arg = getNumCores();
    }
    istringstream(arg) >> numCores;
    arg = getArgMap()["NUMNODES"];
    if (!arg.empty()){
        istringstream(arg) >> numNodes;
    }
    arg = getArgMap()["INFINITE"];
    if (!arg.empty()){
        istringstream(arg) >> isInfinite;
    }

    // Do MPI Initialization
    // Not yet sure what should be passed on in argv
#ifdef USE_MPI
    int argc = 0;
    char ** argv = NULL;
    MPI_Init_thread( &argc, &argv, MPI_THREAD_SERIALIZED, &provided );
    MPI_Comm_size( MPI_COMM_WORLD, &numNodes );
    MPI_Comm_rank( MPI_COMM_WORLD, &myNode );
    if ( provided < MPI_THREAD_SERIALIZED && myNode == 0 ) {
        cout << "Warning: This MPI implementation does not like multithreading: " << provided << "\n";
    }    
#endif
    Msg::initNull();
    // Initialize the shell
    Id shellId;
    vector <unsigned int> dims;
    dims.push_back(1);
    Element * shellE = new Element(shellId, Shell::initCinfo(), "root", dims, 1);
    Id clockId = Id::nextId();
    shell = reinterpret_cast<Shell*>(shellId.eref().data());
    shell->setShellElement(shellE);
    shell->setHardware(isSingleThreaded, numCores, numNodes, myNode);
    shell->loadBalance();
    
    // Initialize the system objects

    new Element(clockId, Clock::initCinfo(), "clock", dims, 1);
    Id tickId( 2 );
    assert(tickId() != 0);
    assert( tickId.value() == 2 );
    assert( tickId()->getName() == "tick" ) ;
    
    Id classMasterId( 3 );
    
    new Element( classMasterId, Neutral::initCinfo(), "classes", dims, 1 );
    
    assert ( shellId == Id() );
    assert( clockId == Id( 1 ) );
    assert( tickId == Id( 2 ) );
    assert( classMasterId == Id( 3 ) );


    /// Sets up the Elements that represent each class of Msg.
    Msg::initMsgManagers();
    
    shell->connectMasterMsg();
    
    Shell::adopt( shellId, clockId );
    Shell::adopt( shellId, classMasterId );
    
    Cinfo::makeCinfoElements( classMasterId );
    
    while ( isInfinite ) // busy loop for debugging under gdb and MPI.
        ;        
    return *shell;        
}



// 
// pymoose.cpp ends here
