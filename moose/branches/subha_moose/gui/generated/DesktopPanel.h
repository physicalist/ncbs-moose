#ifndef _pymoose_DesktopPanel_h
#define _pymoose_DesktopPanel_h
#include "PyMooseBase.h"
class DesktopPanel : public PyMooseBase
{    public:
        static const std::string className;
        DesktopPanel(Id id);
        DesktopPanel(std::string path);
        DesktopPanel(std::string name, Id parentId);
        DesktopPanel(std::string name, PyMooseBase* parent);
        ~DesktopPanel();
        const std::string& getType();
        int __get_x() const;
        void __set_x(int x);
        int __get_y() const;
        void __set_y(int y);
        int __get_w() const;
        void __set_w(int w);
        int __get_h() const;
        void __set_h(int h);
};
#endif
