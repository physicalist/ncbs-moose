#ifndef _pymoose_SymCompartment_cpp
#define _pymoose_SymCompartment_cpp
#include "SymCompartment.h"
using namespace pymoose;
const std::string SymCompartment::className = "SymCompartment";
SymCompartment::SymCompartment(Id id):Compartment(id){}
SymCompartment::SymCompartment(std::string path):Compartment(className, path){}
SymCompartment::SymCompartment(std::string name, Id parentId):Compartment(className, name, parentId){}
SymCompartment::SymCompartment(std::string name, PyMooseBase& parent):Compartment(className, name, parent){}
SymCompartment::SymCompartment(const SymCompartment& src, std::string objectName, PyMooseBase& parent):Compartment(src, objectName, parent){}
SymCompartment::SymCompartment(const SymCompartment& src, std::string objectName, Id& parent):Compartment(src, objectName, parent){}
SymCompartment::SymCompartment(const SymCompartment& src, std::string path):Compartment(src, path){}
SymCompartment::SymCompartment(const Id& src, std::string name, Id& parent):Compartment(src, name, parent){}
SymCompartment::~SymCompartment(){}
const std::string& SymCompartment::getType(){ return className; }
#endif
