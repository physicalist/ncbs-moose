// pymoose.cpp --- 
// 
// Filename: pymoose.cpp
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Copyright (C) 2010 Subhasis Ray, all rights reserved.
// Created: Fri Mar 11 09:50:26 2011 (+0530)
// Version: 
// Last-Updated: Wed Mar 23 11:18:49 2011 (+0530)
//           By: Subhasis Ray
//     Update #: 279
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

void * pymoose_Neutral::getField(string fieldName, char &ftype, unsigned int index)
{
    // The GET_FIELD macro is just a short-cut
#define GET_FIELD(TYPE) (TYPE * ret = new TYPE();       \
                         * ret = Field<TYPE>::get(ObjId(id_, index), fname); \
                         return (void *) ret;)
    
    string type = getFieldType(fieldName);
    ftype = shortType(type);
    switch(ftype){
        case 'c': {
            GET_FIELD(char)
        }
        case 'i': {
            GET_FIELD(int)
        }
        case 'j': {
            GET_FIELD(short)
        }
        case 'l': {
            GET_FIELD(long)
        }
        case 'u': {
            GET_FIELD(unsigned int)
        }
        case 'k': {
            GET_FIELD(unsigned long)
        }
        case 'f': {
            GET_FIELD(float)
        }
        case 'd': {
            GET_FIELD(double)
        }
        case 's': {
            GET_FIELD(string)
        }
        case 'I': {
            GET_FIELD(Id)
        }
        case 'O': {
            GET_FIELD(ObjId)
        }
        case 'D': {
            GET_FIELD(DataId)
        }
        case 'v': {
            GET_FIELD(vector <int>)
        }
        case 'w': {
            GET_FIELD(vector <short>)
        }
        case 'L': {
            GET_FIELD(vector <long>)
        }
        case 'U': {
            GET_FIELD(vector <unsigned int>)
        }
        case 'K': {
            GET_FIELD(vector <unsigned long>)
        }
        case 'F': {
            GET_FIELD(vector <float>)
        }
        case 'x': {
            GET_FIELD(vector <double>)
        }
        case 'S': {
            GET_FIELD(vector <string>)
        }
        case 'J': {
            GET_FIELD(vector <Id>)
        }
        case 'P': {
            GET_FIELD(vector <ObjId>)
        }
        case 'E': {
            GET_FIELD(vector <DataId>)
        }
        default:
            return NULL;
    }
#undef GET_FIELD    
}

int pymoose_Neutral::setField(string fname, void * value, unsigned int index)
{
#define SET_FIELD(TYPE) \
    (TYPE c_val = *((TYPE *)value); \
    Field<TYPE>::set(ObjId(id_, index), fname, c_val); \
    )
    char ftype = shortType(getFieldType(fname));
    switch(ftype){
        case 'c': {
            SET_FIELD(char)
            break;
        }                
        case 'i': {
            SET_FIELD(int)
            break;
        }
        case 'j': {
            SET_FIELD(short)
            break;
        }                
        case 'l': {
            SET_FIELD(long)
            break;
        }                
        case 'u': {
            SET_FIELD(unsigned int)
            break;
        }                
        case 'k': {
            SET_FIELD(unsigned long)
            break;
        }                
        case 'f': {
            SET_FIELD(float)
            break;
        }                
        case 'd': {
            SET_FIELD(double)
            break;
        }                
        case 's': {
            SET_FIELD(string)
            break;
        }                
        case 'I': {
            SET_FIELD(Id)
            break;
        }                
        case 'O': {
            SET_FIELD(ObjId)
            break;
        }                
        case 'D': {
            SET_FIELD(DataId)
            break;
        }                
        case 'C': {
            SET_FIELD(vector <char>)
            break;
        }                
        case 'v': {
            SET_FIELD(vector <int>)
            break;
        }                
        case 'w': {
            SET_FIELD(vector <short>)
            break;
        }                
        case 'L': {
            SET_FIELD(vector <long>)
            break;
        }                
        case 'U': {
            SET_FIELD(vector <unsigned int>)
            break;
        }                
        case 'K': {
            SET_FIELD(vector <unsigned long>)
            break;
        }                
        case 'F': {
            SET_FIELD(vector <float>)
            break;
        }                
        case 'x': {
            SET_FIELD(vector <double>)
            break;
        }                
        case 'S': {
            SET_FIELD(vector <string>)
            break;
        }                
        case 'J': {
            SET_FIELD(vector <Id>)
            break;
        }                
        case 'P': {
            SET_FIELD(vector <ObjId>)
            break;
        }                
        case 'E': {
            SET_FIELD(vector <DataId>)
            break;
        }         
        default:
            return 0;            
    };
    return 1;
#undef SET_FIELD    
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
