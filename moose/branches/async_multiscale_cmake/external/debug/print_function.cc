/*
 * =====================================================================================
 *
 *       Filename:  print_function.cc
 *
 *    Description:  Print function.
 *
 *        Version:  1.0
 *        Created:  07/25/2013 02:48:49 AM
 *       Revision:  none
 *       Compiler:  gcc/g++
 *
 *         Author:  Dilawar Singh (), dilawar@ee.iitb.ac.in
 *   Organization:  IIT Bombay
 *
 * =====================================================================================
 */

#include "print_function.h"


string colored(string msg)
{
  stringstream ss;
  ss << T_RED << msg << T_RESET;
  return ss.str();
}

string colored(string msg, string colorName)
{
  stringstream ss;
  ss << colorName << msg << T_RESET;
  return ss.str();
}

string debugPrint(string msg, string prefix, string color, unsigned debugLevel) 
{
  stringstream ss; ss.str("");
  if(debugLevel <= DEBUG_LEVEL)
  {
    ss << setw(debugLevel/2) << "[" << prefix << "] " 
      << color << msg << T_RESET;
  }
  return ss.str();
}

