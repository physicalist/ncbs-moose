#ifndef _pymoose_Neutral_h
#define _pymoose_Neutral_h
#include "PyMooseBase.h"
class Neutral : public PyMooseBase
{    public:
        static const std::string className;
        Neutral(Id id);
        Neutral(std::string path);
        Neutral(std::string name, Id parentId);
        Neutral(std::string name, PyMooseBase* parent);
        ~Neutral();
        const std::string& getType();
        string __get_name() const;
        void __set_name(string name);
        2Id __get_parent() const;
        void __set_parent(2Id parent);
        string __get_class() const;
        void __set_class(string class);
        St6vectorI2IdSaIS0_EE __get_childList() const;
        void __set_childList(St6vectorI2IdSaIS0_EE childList);
        double __get_cpu() const;
        void __set_cpu(double cpu);
        unsigned int __get_dataMem() const;
        void __set_dataMem(unsigned int dataMem);
        unsigned int __get_msgMem() const;
        void __set_msgMem(unsigned int msgMem);
        2Id,string __get_lookupChild() const;
        void __set_lookupChild(2Id,string lookupChild);
        St6vectorISsSaISsEE __get_fieldList() const;
        void __set_fieldList(St6vectorISsSaISsEE fieldList);
        int __get_childSrc() const;
        void __set_childSrc(int childSrc);
        int __get_child() const;
        void __set_child(int child);
        string,string __get_create() const;
        void __set_create(string,string create);
        none __get_destroy() const;
        void __set_destroy(none destroy);
        none __get_postCreate() const;
        void __set_postCreate(none postCreate);
};
#endif
