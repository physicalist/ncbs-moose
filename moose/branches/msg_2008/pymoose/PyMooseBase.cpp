/*******************************************************************
 * File:            PyMooseBase.cpp
 * Description:     Implementation of the PyMooseBase class. 
 * Author:          Subhasis Ray / NCBS
 * Created:         2007-03-10 18:42:50
 ********************************************************************/
#ifndef _PYMOOSE_BASE_CPP
#define _PYMOOSE_BASE_CPP

#include "PyMooseBase.h"
#include "../shell/ReadCell.h"
#include <iostream>
#include <sstream>
using namespace std;
using namespace pymoose;
    
string PyMooseBase::separator_ = "/"; /// this is default separator is the unix path separator

PyMooseContext* PyMooseBase::context_ = PyMooseContext::createPyMooseContext("BaseContext", "shell");

/**
   This constructor is protected and is intended for use by the
   subclasses only.  When a you need access to existing moose object
   in the pymoose way, wrap the object inside an instance of any class
   extending PyMooseBase (although it is advisable that you
   wrap it inside only a class that is the same class as the moose
   object itself, pymoose does not complain. But be ready for surprise
   if you wrap a Table inside a Compartment class. Most likely you will
   get a segmentation violation error. Don't say I did not tell you!)

   @param id - Id of the object to be wrapped.
*/
PyMooseBase::PyMooseBase(Id id)
{
    Element* e = id();
    if ( e == NULL)
    {
        cerr << "ERROR: PyMooseBase::PyMooseBase(Id id) - this Id does not exist." << endl;
        id_ = Id();    
        return;
    }
    id_ = id;
}
/**
   This is the most commonly used constructor. It will create an
   object of the given class under the given parent and the object
   will be named as the objectName passed to this constructor.
   
   @param className - name of the class that is actually being
   instantiated.
   
   @param objectName - name of the object to be created. The path to
   the object will be ${PATH_TO_PARENT}/objectName

   @param parentId - Id of the parent of this object. In the object
   tree, the newly instantiated object will be placed under the object
   with id parentId.
*/
PyMooseBase::PyMooseBase(std::string className, std::string objectName, Id parentId )
{
    if ( objectName.find(getSeparator()) != std::string::npos)
    {
        cerr << "Error: PyMooseBase::PyMooseBase(std::string className, std::string objectName, Id parentId ) - object name should not have " << getSeparator() << endl;
        id_ = Id::badId();
        return;        
    }
    string path = parentId.path(getSeparator())+getSeparator()+objectName;
    
    id_ = PyMooseBase::pathToId(path,false);
    if (!id_.bad())
    {
        cerr << "Info: PyMooseBase::PyMooseBase(" << className << ", " <<  path << ") - ";
        
        if ( path == getSeparator() || path == (getSeparator()+"root"))
        {
            cerr << "Returning the predefined root element." << endl;
        } else 
        {
            cerr << "Returning already existing object." << endl;
        }
        return;
    }

    id_ = context_->create(className, objectName, parentId);    
}

/**
   This constructor is for convenience. If you ar more comfortable
   with viewing the moose object tree as a unix file system tree, this
   constructor is for you. Just specify a full valid path, and voila,
   your object is there! No need to fool around with the id's. But you
   need to provide a valid path. Make sure you know it.

   @param className - name of the class to be actually instantiated.

   @param path - full path of the object. If the path to the parent is
   ${PATH_TO_PARENT} and you want the new object to be called child,
   then the path to be specified is: ${PATH_TO_PARENT}/child.
*/
PyMooseBase::PyMooseBase(std::string className, std::string path)
{
     
    assert(path.length() > 0);
    // Consider the pros and cons - do we want to check pre existence for each new create call?
    // But I personally tend to make the mistake of trying to wrap an existing path inside a new python object
    // table_A = moose.InterpolationTable(channel.getPath+'/xGate/A') - which fails otherwise
    
    // 2007-07-30: returning pre-existing objects with an info message seems to be the best thing
    // Try to wrap preexisting object
    id_ = PyMooseBase::pathToId(path,false);
    if (!id_.bad())
    {
        cerr << "Info: PyMooseBase::PyMooseBase(" << className << ", " <<  path << ") - ";
        
        if ( path == getSeparator() || path == (getSeparator()+"root"))
        {
            cerr << "Returning the predefined root element." << endl;
        } else 
        {
            cerr << "Returning already existing object." << endl;
        }
        return;
    }

    // Extract the object name from the path
    std::string::size_type name_start = path.rfind(getSeparator(),path.length()-1);
    std::string myName;    
    std::string parentPath;
    
    if (name_start == std::string::npos)
    {
        name_start = 0;
        myName = path;
        Id parentId = context_->getCwe();
        id_ = context_->create(className, myName, parentId );
    }
    else 
    {
        myName = path.substr(name_start+1);
        parentPath = path.substr(0,name_start);
        Id parentId = context_->pathToId(parentPath, false);
        id_ = context_->create(className, myName, parentId );
    }
    

}

/**
   Who wants to extract the Id of each object before creating its
   child? Just pass the object as parent.

   @param className - name of the class to be instantiated.
   
   @param objectName - name of the object to be created.

   @param parent - the object under which the newly instantiated
   object will be placed.
*/
PyMooseBase::PyMooseBase(std::string className, std::string objectName, PyMooseBase& parent)
{
    if ( parent.id_.bad() )
    {
        cerr << "Error: Invalid parent Id" << endl;
        id_ = Id::badId();        
        return;
    }
    
    string path = parent.path() + getSeparator() + objectName;
    id_ = PyMooseBase::pathToId(path,false);
    if (!id_.bad())
    {
        cerr << "Info: PyMooseBase::PyMooseBase(" << className << ", " <<  path << ") - ";
        
        if ( path == getSeparator() || path == (getSeparator()+"root"))
        {
            cerr << "Returning the predefined root element." << endl;
        } else 
        {
            cerr << "Returning already existing object." << endl;
        }
        return;
    }
    id_ = context_->create(className, objectName, parent.id_);
}

/**
   This constructor wraps read-cell -- reading the object from an ascii text file   
*/
PyMooseBase::PyMooseBase(std::string className, std::string path, std::string fileName)
{
    //ReadCell reader;
    //reader.read(fileName, path);
    id_ = context_->pathToId(path, false);
}


/**
   I still have a big dilemma with the destructor. Should an object be
   _actually_ deleted in moose environment when the pymoose destructor
   is called, or should it be allowed to live happily until the end of
   the moose world. There are some pros and cons.

   
   We do not want the objects to be really destroyed until the
   simulation is over and user asks for destruction explicitly.  So if
   we called the destroy method here, we would have had problem when
   the reference count for something in python wen to zero, e.g.,
   
   c = moose.Compartment("test")
   for i in range(10):
   d = moose.Compartment("test_"+str(i), c)

   This results in the destructor being called in each iteration of
   loop and only the 10-th child survives. Hence we move the actual
   destruction to a separate method which is supposed to be called
   explicitly at the end of simulation.

   On the other hand, the law of  least surprise will 
*/
PyMooseBase::~PyMooseBase()
{
    id_ = Id();    
}

const std::string& PyMooseBase::getSeparator() const
{
    return context_->separator;    
}
const std::string  PyMooseBase::path() const 
{
    return context_->getPath(id_);    
}

const Id* PyMooseBase::__get_id() const
{
    return &id_;
}

const Id* PyMooseBase::__get_parent() const 
{
    return &context_->getParent(id_);
}

vector <Id>& PyMooseBase::__get_children() const
{
    return context_->getChildren(id_);
}

bool PyMooseBase::connect(std::string srcField, PyMooseBase* dest, std::string destField)
{
    return context_->connect(this->id_, srcField, dest->id_, destField);    
}

bool PyMooseBase::connect(std::string srcField, Id dest, std::string destField)
{
    return context_->connect(this->id_, srcField, dest, destField);    
}

PyMooseContext* PyMooseBase::getContext()
{
    return context_;
}

/**
 * listMessages builds a list of messages associated with the 
 * specified element on the named field, and sends it back to
 * the calling parser. It extracts the
 * target element from the connections, and puts this into a
 * vector of unsigned ints.
 */
vector <std::string> PyMooseBase::getMessageList(string field, bool isIncoming )
{
    assert( !id_.bad() );
    Element* e = id_();
    const Finfo* f = e->findFinfo( field );
    assert( f != 0 );
    vector< Conn > list;
    vector< Id > ret;
    vector <std::string> remoteFields;
    std::string arrow = "";
    if ( isIncoming )
    {
        f->incomingConns( e, list );
        arrow = " <- ";        
    }        
    else
    {
        f->outgoingConns( e, list );
        arrow = " -> ";        
    }
        
    if ( list.size() > 0 ) {
        vector< Conn >::iterator i;
        for ( i = list.begin(); i != list.end(); i++ ) {
            Element* temp = i->targetElement();
            const Finfo* targetFinfo = 
                temp->findFinfo( i->targetIndex() );
            assert( targetFinfo != 0 );
            string line = field + arrow + "[ "+ (temp->id()).path()+" ]."+targetFinfo->name();            
            remoteFields.push_back( line);
        }
    }
    return remoteFields;
}
/**
   Wraps getMessageList and presents incoming messages as a field to python.
*/
vector <std::string>& PyMooseBase::__get_incoming_messages()
{

    vector <std::string> fieldList;
    string fields = context_->getField(id_,"fieldList" );
    separateString( fields, fieldList, ", " );
    incomingMessages_.clear();
    
    for ( unsigned int i = 0; i < fieldList.size(); ++i )
    {
        if ( fieldList[i] != "fieldList" ){
            vector <std::string> tmpList = getMessageList(fieldList[i], true);
            for ( unsigned int j = 0; j < tmpList.size(); ++j )
            {
                incomingMessages_.push_back(tmpList[j]);
            }
        }
    }
    return incomingMessages_;    
}
/*
  Wraps getMessageList and presents outgoing messages as a field to python.
*/
vector <std::string>& PyMooseBase::__get_outgoing_messages()
{
    
    vector <std::string> fieldList;
    string fields = context_->getField(id_,"fieldList" );
    separateString( fields, fieldList, ", " );
    outgoingMessages_.clear();
    
    for ( unsigned int i = 0; i < fieldList.size(); ++i )
    {
        if ( fieldList[i] != "fieldList" ){
            vector <std::string> tmpList = getMessageList(fieldList[i], false);
            for ( unsigned int j = 0; j < tmpList.size(); ++j )
            {
                outgoingMessages_.push_back(tmpList[j]);
            }
        }
    }
    return outgoingMessages_;
}

/**
   This method is for reinitializing simulation once
   PyMooseBase::endSimulation() method has been called and
   PyMooseBase::context_ has become invalid. Otherwise it does not do
   anything.
*/
void PyMooseBase::initSimulation()
{
    if (context_== NULL)
    {
        context_ = PyMooseContext::createPyMooseContext("BaseContext", "shell");
    }    
}
/**
   This method destroys all objects that have been created under the
   current context and deletes the context itself
*/
void PyMooseBase::endSimulation()
{
    context_->end();
    // \NOTE: not sure whether we should handle the destruction of context object inside python
    //    PyMooseContext::destroyPyMooseContext(context_);    
    //    context_ = NULL;    
}

// These functions break the Object-oriented design.
// But these are here for facility of the user and to handle
// orphans and the root element
Id PyMooseBase::getParent(Id id)
{
    return context_->getParent(id);
}

vector <Id>& PyMooseBase::getChildren(Id id)
{
    return context_->getChildren(id);
}

bool PyMooseBase::exists(Id id)
{
    return context_->exists(id);    
}

bool PyMooseBase::exists(std::string path)
{
    return context_->exists(path);    
}

vector <Id>& PyMooseBase::le()
{
    return context_->getChildren(context_->getCwe());
}

Id PyMooseBase::pwe()
{
    return context_->getCwe();
}

Id PyMooseBase::ce(Id newElement)
{
    context_->setCwe(newElement);
    return newElement;    
}
Id PyMooseBase::ce(std::string path)
{
    Id id = context_->pathToId(path);
    context_->setCwe(id);
    return id;    
}
Id PyMooseBase::pathToId(std::string path, bool echo)
{
    Id id = context_->pathToId(path, echo);
    return id;    
}
const std::string PyMooseBase::idToPath(Id id)
{
    return context_->getPath(id);
}

bool PyMooseBase::destroy(Id id)
{
    return context_->destroy(id);    
}

#endif
