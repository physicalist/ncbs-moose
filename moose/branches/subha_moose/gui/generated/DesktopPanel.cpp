#ifndef _pymoose_DesktopPanel_cpp
#define _pymoose_DesktopPanel_cpp
#include "DesktopPanel.h"
const std::string DesktopPanel::className = "DesktopPanel";
DesktopPanel::DesktopPanel(Id id):PyMooseBase(id){}
DesktopPanel::DesktopPanel(std::string path):PyMooseBase(className, path){}
DesktopPanel::DesktopPanel(std::string name, Id parentId):PyMooseBase(className, name, parentId){}
DesktopPanel::DesktopPanel(std::string name, PyMooseBase* parent):PyMooseBase(className, name, parent){}
DesktopPanel::~DesktopPanel(){}
const std::string& DesktopPanel::getType(){ return className; }
int DesktopPanel::__get_x() const
{
    int x;
    get < int > (id_(), "x",x);
    return x;
}
void DesktopPanel::__set_x( int x )
{
    set < int > (id_(), "x", x);
}
int DesktopPanel::__get_y() const
{
    int y;
    get < int > (id_(), "y",y);
    return y;
}
void DesktopPanel::__set_y( int y )
{
    set < int > (id_(), "y", y);
}
int DesktopPanel::__get_w() const
{
    int w;
    get < int > (id_(), "w",w);
    return w;
}
void DesktopPanel::__set_w( int w )
{
    set < int > (id_(), "w", w);
}
int DesktopPanel::__get_h() const
{
    int h;
    get < int > (id_(), "h",h);
    return h;
}
void DesktopPanel::__set_h( int h )
{
    set < int > (id_(), "h", h);
}
#endif
