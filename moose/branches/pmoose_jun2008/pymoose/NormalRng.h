#ifndef _pymoose_NormalRng_h
#define _pymoose_NormalRng_h
#include "RandGenerator.h"
namespace pymoose
{
    class NormalRng : public RandGenerator
    {    public:
        static const std::string className;
        NormalRng(Id id);
        NormalRng(std::string path);
        NormalRng(std::string name, Id parentId);
        NormalRng(std::string name, PyMooseBase& parent);
        NormalRng(const NormalRng& src,std::string name, PyMooseBase& parent);
        NormalRng(const NormalRng& src,std::string name, Id& parent);
        NormalRng(const Id& src,std::string name, Id& parent);
        NormalRng(const NormalRng& src,std::string path);
        ~NormalRng();
        const std::string& getType();
        double __get_mean() const;
        void __set_mean(double mean);
        double __get_variance() const;
        void __set_variance(double variance);
        int __get_method() const;
        void __set_method(int method);
    };
}

#endif
