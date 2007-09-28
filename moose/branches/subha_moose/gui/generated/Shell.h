#ifndef _pymoose_Shell_h
#define _pymoose_Shell_h
#include "PyMooseBase.h"
class Shell : public PyMooseBase
{    public:
        static const std::string className;
        Shell(Id id);
        Shell(std::string path);
        Shell(std::string name, Id parentId);
        Shell(std::string name, PyMooseBase* parent);
        ~Shell();
        const std::string& getType();
        2Id __get_cwe() const;
        void __set_cwe(2Id cwe);
        string __get_xrawAdd() const;
        void __set_xrawAdd(string xrawAdd);
        none __get_poll() const;
        void __set_poll(none poll);
        int __get_pollSrc() const;
        void __set_pollSrc(int pollSrc);
};
#endif
