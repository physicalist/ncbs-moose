#ifndef _pymoose_Sched1_h
#define _pymoose_Sched1_h
#include "PyMooseBase.h"
class Sched1 : public PyMooseBase
{    public:
        static const std::string className;
        Sched1(Id id);
        Sched1(std::string path);
        Sched1(std::string name, Id parentId);
        Sched1(std::string name, PyMooseBase* parent);
        ~Sched1();
        const std::string& getType();
};
#endif
