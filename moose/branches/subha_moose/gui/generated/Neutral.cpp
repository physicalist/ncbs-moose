#ifndef _pymoose_Neutral_cpp
#define _pymoose_Neutral_cpp
#include "Neutral.h"
const std::string Neutral::className = "Neutral";
Neutral::Neutral(Id id):PyMooseBase(id){}
Neutral::Neutral(std::string path):PyMooseBase(className, path){}
Neutral::Neutral(std::string name, Id parentId):PyMooseBase(className, name, parentId){}
Neutral::Neutral(std::string name, PyMooseBase* parent):PyMooseBase(className, name, parent){}
Neutral::~Neutral(){}
const std::string& Neutral::getType(){ return className; }
string Neutral::__get_name() const
{
    string name;
    get < string > (id_(), "name",name);
    return name;
}
void Neutral::__set_name( string name )
{
    set < string > (id_(), "name", name);
}
2Id Neutral::__get_parent() const
{
    2Id parent;
    get < 2Id > (id_(), "parent",parent);
    return parent;
}
void Neutral::__set_parent( 2Id parent )
{
    set < 2Id > (id_(), "parent", parent);
}
string Neutral::__get_class() const
{
    string class;
    get < string > (id_(), "class",class);
    return class;
}
void Neutral::__set_class( string class )
{
    set < string > (id_(), "class", class);
}
St6vectorI2IdSaIS0_EE Neutral::__get_childList() const
{
    St6vectorI2IdSaIS0_EE childList;
    get < St6vectorI2IdSaIS0_EE > (id_(), "childList",childList);
    return childList;
}
void Neutral::__set_childList( St6vectorI2IdSaIS0_EE childList )
{
    set < St6vectorI2IdSaIS0_EE > (id_(), "childList", childList);
}
double Neutral::__get_cpu() const
{
    double cpu;
    get < double > (id_(), "cpu",cpu);
    return cpu;
}
void Neutral::__set_cpu( double cpu )
{
    set < double > (id_(), "cpu", cpu);
}
unsigned int Neutral::__get_dataMem() const
{
    unsigned int dataMem;
    get < unsigned int > (id_(), "dataMem",dataMem);
    return dataMem;
}
void Neutral::__set_dataMem( unsigned int dataMem )
{
    set < unsigned int > (id_(), "dataMem", dataMem);
}
unsigned int Neutral::__get_msgMem() const
{
    unsigned int msgMem;
    get < unsigned int > (id_(), "msgMem",msgMem);
    return msgMem;
}
void Neutral::__set_msgMem( unsigned int msgMem )
{
    set < unsigned int > (id_(), "msgMem", msgMem);
}
2Id,string Neutral::__get_lookupChild() const
{
    2Id,string lookupChild;
    get < 2Id,string > (id_(), "lookupChild",lookupChild);
    return lookupChild;
}
void Neutral::__set_lookupChild( 2Id,string lookupChild )
{
    set < 2Id,string > (id_(), "lookupChild", lookupChild);
}
St6vectorISsSaISsEE Neutral::__get_fieldList() const
{
    St6vectorISsSaISsEE fieldList;
    get < St6vectorISsSaISsEE > (id_(), "fieldList",fieldList);
    return fieldList;
}
void Neutral::__set_fieldList( St6vectorISsSaISsEE fieldList )
{
    set < St6vectorISsSaISsEE > (id_(), "fieldList", fieldList);
}
int Neutral::__get_childSrc() const
{
    int childSrc;
    get < int > (id_(), "childSrc",childSrc);
    return childSrc;
}
void Neutral::__set_childSrc( int childSrc )
{
    set < int > (id_(), "childSrc", childSrc);
}
int Neutral::__get_child() const
{
    int child;
    get < int > (id_(), "child",child);
    return child;
}
void Neutral::__set_child( int child )
{
    set < int > (id_(), "child", child);
}
string,string Neutral::__get_create() const
{
    string,string create;
    get < string,string > (id_(), "create",create);
    return create;
}
void Neutral::__set_create( string,string create )
{
    set < string,string > (id_(), "create", create);
}
none Neutral::__get_destroy() const
{
    none destroy;
    get < none > (id_(), "destroy",destroy);
    return destroy;
}
void Neutral::__set_destroy( none destroy )
{
    set < none > (id_(), "destroy", destroy);
}
none Neutral::__get_postCreate() const
{
    none postCreate;
    get < none > (id_(), "postCreate",postCreate);
    return postCreate;
}
void Neutral::__set_postCreate( none postCreate )
{
    set < none > (id_(), "postCreate", postCreate);
}
#endif
