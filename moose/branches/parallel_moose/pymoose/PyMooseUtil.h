/*******************************************************************
 * File:            PyMooseUtil.h
 * Description:     This contains some utility functions for use in
 *                  python interface of MOOSE.
 * Author:          Subhasis Ray / NCBS
 * Created:         2007-03-23 16:06:11
 ********************************************************************/
#ifndef _PYMOOSE_UTIL_H
#define _PYMOOSE_UTIL_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
template <typename T>
std::string toString( T data)
{
    std::string value;
    std::ostringstream s;
    
    if (s << data)
    {
        return s.str();
    }
    else 
    {
        return 0;
    }    
}
bool isEqual(double , double, double);

#endif
