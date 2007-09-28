/*******************************************************************
 * File:            TestGui.cpp
 * Description:      
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-08-03 18:24:19
 ********************************************************************/
#ifdef DO_UNIT_TESTS

#include <fstream>
#include "header.h"

extern void testDesktopPanel(int argc, char **argv); // Defined in Desktoppanel.Cpp

void testGui()
{
    testDesktopPanel(0, 0);    
}

#endif


