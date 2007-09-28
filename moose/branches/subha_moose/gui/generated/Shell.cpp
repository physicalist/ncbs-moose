#ifndef _pymoose_Shell_cpp
#define _pymoose_Shell_cpp
#include "Shell.h"
const std::string Shell::className = "Shell";
Shell::Shell(Id id):PyMooseBase(id){}
Shell::Shell(std::string path):PyMooseBase(className, path){}
Shell::Shell(std::string name, Id parentId):PyMooseBase(className, name, parentId){}
Shell::Shell(std::string name, PyMooseBase* parent):PyMooseBase(className, name, parent){}
Shell::~Shell(){}
const std::string& Shell::getType(){ return className; }
2Id Shell::__get_cwe() const
{
    2Id cwe;
    get < 2Id > (id_(), "cwe",cwe);
    return cwe;
}
void Shell::__set_cwe( 2Id cwe )
{
    set < 2Id > (id_(), "cwe", cwe);
}
string Shell::__get_xrawAdd() const
{
    string xrawAdd;
    get < string > (id_(), "xrawAdd",xrawAdd);
    return xrawAdd;
}
void Shell::__set_xrawAdd( string xrawAdd )
{
    set < string > (id_(), "xrawAdd", xrawAdd);
}
none Shell::__get_poll() const
{
    none poll;
    get < none > (id_(), "poll",poll);
    return poll;
}
void Shell::__set_poll( none poll )
{
    set < none > (id_(), "poll", poll);
}
int Shell::__get_pollSrc() const
{
    int pollSrc;
    get < int > (id_(), "pollSrc",pollSrc);
    return pollSrc;
}
void Shell::__set_pollSrc( int pollSrc )
{
    set < int > (id_(), "pollSrc", pollSrc);
}
#endif
