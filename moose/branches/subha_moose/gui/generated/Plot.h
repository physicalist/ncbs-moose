#ifndef _pymoose_Plot_h
#define _pymoose_Plot_h
#include "PyMooseBase.h"
class Plot : public PyMooseBase
{    public:
        static const std::string className;
        Plot(Id id);
        Plot(std::string path);
        Plot(std::string name, Id parentId);
        Plot(std::string name, PyMooseBase* parent);
        ~Plot();
        const std::string& getType();
        bool __get_Yfx() const;
        void __set_Yfx(bool Yfx);
        string __get_color() const;
        void __set_color(string color);
        string __get_style() const;
        void __set_style(string style);
        int __get_width() const;
        void __set_width(int width);
        string __get_symbol() const;
        void __set_symbol(string symbol);
};
#endif
