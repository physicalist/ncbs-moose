#ifndef _pymoose_PoissonRng_cpp
#define _pymoose_PoissonRng_cpp
#include "PoissonRng.h"
using namespace pymoose;
const std::string PoissonRng::className = "PoissonRng";
PoissonRng::PoissonRng(Id id):RandGenerator(id){}
PoissonRng::PoissonRng(std::string path):RandGenerator(className, path){}
PoissonRng::PoissonRng(std::string name, Id parentId):RandGenerator(className, name, parentId){}
PoissonRng::PoissonRng(std::string name, PyMooseBase& parent):RandGenerator(className, name, parent){}
PoissonRng::PoissonRng(const PoissonRng& src, std::string objectName,  PyMooseBase& parent):RandGenerator(src, objectName, parent){}

PoissonRng::PoissonRng(const PoissonRng& src, std::string objectName, Id& parent):RandGenerator(src, objectName, parent){}
PoissonRng::PoissonRng(const PoissonRng& src, std::string path):RandGenerator(src, path)
{
}

PoissonRng::PoissonRng(const Id& src, string name, Id& parent):RandGenerator(src, name, parent)
{
}
PoissonRng::~PoissonRng(){}
const std::string& PoissonRng::getType(){ return className; }
double PoissonRng::__get_mean() const
{
    double mean;
    get < double > (id_(), "mean",mean);
    return mean;
}
void PoissonRng::__set_mean( double mean )
{
    set < double > (id_(), "mean", mean);
}
#endif
