#ifndef _pymoose_KinCompt_cpp
#define _pymoose_KinCompt_cpp
#include "KinCompt.h"
using namespace pymoose;
const std::string KinCompt::className = "KinCompt";
KinCompt::KinCompt(Id id):PyMooseBase(id){}
KinCompt::KinCompt(std::string path):PyMooseBase(className, path){}
KinCompt::KinCompt(std::string name, Id parentId):PyMooseBase(className, name, parentId){}
KinCompt::KinCompt(std::string name, PyMooseBase& parent):PyMooseBase(className, name, parent){}
KinCompt::KinCompt(const KinCompt& src, std::string objectName, PyMooseBase& parent):PyMooseBase(src, objectName, parent){}
KinCompt::KinCompt(const KinCompt& src, std::string objectName, Id& parent):PyMooseBase(src, objectName, parent){}
KinCompt::KinCompt(const KinCompt& src, std::string path):PyMooseBase(src, path){}
KinCompt::KinCompt(const Id& src, std::string name, Id& parent):PyMooseBase(src, name, parent){}
KinCompt::~KinCompt(){}
const std::string& KinCompt::getType(){ return className; }
double KinCompt::__get_volume() const
{
    double volume;
    get < double > (id_(), "volume",volume);
    return volume;
}
void KinCompt::__set_volume( double volume )
{
    set < double > (id_(), "volume", volume);
}
double KinCompt::__get_area() const
{
    double area;
    get < double > (id_(), "area",area);
    return area;
}
void KinCompt::__set_area( double area )
{
    set < double > (id_(), "area", area);
}
double KinCompt::__get_perimeter() const
{
    double perimeter;
    get < double > (id_(), "perimeter",perimeter);
    return perimeter;
}
void KinCompt::__set_perimeter( double perimeter )
{
    set < double > (id_(), "perimeter", perimeter);
}
double KinCompt::__get_size() const
{
    double size;
    get < double > (id_(), "size",size);
    return size;
}
void KinCompt::__set_size( double size )
{
    set < double > (id_(), "size", size);
}
unsigned int KinCompt::__get_numDimensions() const
{
    unsigned int numDimensions;
    get < unsigned int > (id_(), "numDimensions",numDimensions);
    return numDimensions;
}
void KinCompt::__set_numDimensions( unsigned int numDimensions )
{
    set < unsigned int > (id_(), "numDimensions", numDimensions);
}
#endif
