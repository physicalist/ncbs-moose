#ifndef MAIN_CPP_
#define MAIN_CPP
#include "DesktopPanel.h"
#include "Plot.h"
#include <iostream>
#include <cmath>
using namespace Qt;

using std::cerr;
using std::endl;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    DesktopPanel desktop;
    desktop.init();    
    desktop.resize(600,400);    
    desktop.show();
    return app.exec();    
}

#endif
