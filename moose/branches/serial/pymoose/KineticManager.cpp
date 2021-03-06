#ifndef _pymoose_KineticManager_cpp
#define _pymoose_KineticManager_cpp
#include "KineticManager.h"
using namespace pymoose;
const std::string KineticManager::className = "KineticManager";
KineticManager::KineticManager(Id id):PyMooseBase(id){}
KineticManager::KineticManager(std::string path):PyMooseBase(className, path){}
KineticManager::KineticManager(std::string name, Id parentId):PyMooseBase(className, name, parentId){}
KineticManager::KineticManager(std::string name, PyMooseBase& parent):PyMooseBase(className, name, parent){}
KineticManager::KineticManager(
    const KineticManager& src,
    std::string name,
    PyMooseBase& parent)
    :PyMooseBase(src, name, parent)
{
}

KineticManager::KineticManager(
    const KineticManager& src,
    std::string name,
    Id& parent)
 :PyMooseBase(src, name, parent)
{
}

KineticManager::KineticManager(
    const KineticManager& src,
    std::string path)
 :PyMooseBase(src, path)
{
}

KineticManager::KineticManager(
    const Id& src,
    std::string name,
    Id& parent)
 :PyMooseBase(src, name, parent)
{
}

KineticManager::~KineticManager(){}

const std::string& KineticManager::getType(){ return className; }
bool KineticManager::__get_autoMode() const
{
    bool _auto;
    get < bool > (id_(), "auto",_auto);
    return _auto;
}
void KineticManager::__set_autoMode( bool _auto )
{
    set < bool > (id_(), "auto", _auto);
}
bool KineticManager::__get_stochastic() const
{
    bool stochastic;
    get < bool > (id_(), "stochastic",stochastic);
    return stochastic;
}
void KineticManager::__set_stochastic( bool stochastic )
{
    set < bool > (id_(), "stochastic", stochastic);
}
bool KineticManager::__get_spatial() const
{
    bool spatial;
    get < bool > (id_(), "spatial",spatial);
    return spatial;
}
void KineticManager::__set_spatial( bool spatial )
{
    set < bool > (id_(), "spatial", spatial);
}
string KineticManager::__get_method() const
{
    string method;
    get < string > (id_(), "method",method);
    return method;
}
void KineticManager::__set_method( string method )
{
    set < string > (id_(), "method", method);
}
bool KineticManager::__get_variableDt() const
{
    bool variableDt;
    get < bool > (id_(), "variableDt",variableDt);
    return variableDt;
}
bool KineticManager::__get_singleParticle() const
{
    bool singleParticle;
    get < bool > (id_(), "singleParticle",singleParticle);
    return singleParticle;
}
bool KineticManager::__get_multiscale() const
{
    bool multiscale;
    get < bool > (id_(), "multiscale",multiscale);
    return multiscale;
}
bool KineticManager::__get_implicit() const
{
    bool implicit;
    get < bool > (id_(), "implicit",implicit);
    return implicit;
}
string KineticManager::__get_description() const
{
    string description;
    get < string > (id_(), "description",description);
    return description;
}
double KineticManager::__get_recommendedDt() const
{
    double recommendedDt;
    get < double > (id_(), "recommendedDt",recommendedDt);
    return recommendedDt;
}
double KineticManager::__get_eulerError() const
{
    double eulerError;
    get < double > (id_(), "eulerError",eulerError);
    return eulerError;
}
void KineticManager::__set_eulerError( double eulerError )
{
    set < double > (id_(), "eulerError", eulerError);
}
#endif
