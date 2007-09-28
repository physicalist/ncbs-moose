#ifndef _pymoose_Plot_cpp
#define _pymoose_Plot_cpp
#include "Plot.h"
const std::string Plot::className = "Plot";
Plot::Plot(Id id):PyMooseBase(id){}
Plot::Plot(std::string path):PyMooseBase(className, path){}
Plot::Plot(std::string name, Id parentId):PyMooseBase(className, name, parentId){}
Plot::Plot(std::string name, PyMooseBase* parent):PyMooseBase(className, name, parent){}
Plot::~Plot(){}
const std::string& Plot::getType(){ return className; }
bool Plot::__get_Yfx() const
{
    bool Yfx;
    get < bool > (id_(), "Yfx",Yfx);
    return Yfx;
}
void Plot::__set_Yfx( bool Yfx )
{
    set < bool > (id_(), "Yfx", Yfx);
}
string Plot::__get_color() const
{
    string color;
    get < string > (id_(), "color",color);
    return color;
}
void Plot::__set_color( string color )
{
    set < string > (id_(), "color", color);
}
string Plot::__get_style() const
{
    string style;
    get < string > (id_(), "style",style);
    return style;
}
void Plot::__set_style( string style )
{
    set < string > (id_(), "style", style);
}
int Plot::__get_width() const
{
    int width;
    get < int > (id_(), "width",width);
    return width;
}
void Plot::__set_width( int width )
{
    set < int > (id_(), "width", width);
}
string Plot::__get_symbol() const
{
    string symbol;
    get < string > (id_(), "symbol",symbol);
    return symbol;
}
void Plot::__set_symbol( string symbol )
{
    set < string > (id_(), "symbol", symbol);
}
#endif
