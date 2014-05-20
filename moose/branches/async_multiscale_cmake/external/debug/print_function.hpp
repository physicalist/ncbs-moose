/*
 * =====================================================================================
 *
 *       Filename:  print_function.h
 *
 *    Description:  Some of basic print routines for c++
 *
 *        Version:  1.0
 *        Created:  07/18/2013 07:34:06 PM
 *       Revision:  none
 *       Compiler:  gcc/g++
 *
 *         Author:  Dilawar Singh (), dilawar@ee.iitb.ac.in
 *   Organization:  IIT Bombay
 *
 * =====================================================================================
 */

#ifndef  print_function_INC
#define  print_function_INC

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <sys/ioctl.h>

#define T_RESET       "\033[0m"
#define T_BLACK       "\033[30m"      /* Black */
#define T_RED         "\033[31m"      /* Red */
#define T_GREEN       "\033[32m"      /* Green */
#define T_YELLOW      "\033[33m"      /* Yellow */
#define T_BLUE        "\033[34m"      /* Blue */
#define T_MAGENTA     "\033[35m"      /* Magenta */
#define T_CYAN        "\033[36m"      /* Cyan */
#define T_WHITE       "\033[37m"      /* White */
#define T_BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define T_BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define T_BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define T_BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define T_BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define T_BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define T_BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define T_BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

using namespace std;


string debugPrint(string msg, string prefix, string color = T_RESET
        , unsigned indentLevel = 10);


string colored(string msg);

string colored(string msg, string colorName);

/*  pretty print onto console using colors. */
void dump(string msg, string type="WARNING", bool autoFormat=true);

/*  log to a file and also write to console. */
void log(string msg, string type, bool redirectToConsole=true
        , bool removeTicks=true);

/* Check if a given character is a backtick ` */
bool isBackTick(char a);

/* 
 * ===  FUNCTION  ==============================================================
 *         Name:  mapToString
 *  Description:  GIven a map, return a string representation of it.
 *
 *  If the second argument is true then print the value with key. But default it
 *  is true.
 * ==============================================================================
 */
    template<typename A, typename B>
string mapToString(const map<A, B>& m, bool value=true)
{
    unsigned int width = 80;
    unsigned int mapSize = m.size();
    unsigned int size = 0;

    vector<string> row;

    /* Get the maximum size of any entry in map */
    stringstream ss;
    typename map<A, B>::const_iterator it;
    for(it = m.begin(); it != m.end(); it++)
    {
        ss.str("");
        ss << it->first;
        if(value)
            ss << ": `" << it->second << '`';
        row.push_back(ss.str());
        if(ss.str().size() > size)
            size = ss.str().size()+1;
    }

    unsigned int colums = width / size;
    ss.str("");

    int i = 0;
    for(unsigned int ii = 0; ii < row.size(); ii++)
    {
        if(i < colums)
        {
            ss << setw(size) << row[ii];
            i++;
        }
        else
        {
            ss << endl;
            i = 0;
        }
    }
    return ss.str();
}

#include <ctime>
#include <algorithm>

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

/*-----------------------------------------------------------------------------
 *  This function dumps a message onto console. Fills appropriate colors as
 *  needed. What can I do, I love colors.
 *-----------------------------------------------------------------------------*/
void dump(string msg, string type, bool autoFormat)
{
    stringstream ss;
    ss << "[" << type << "] ";
    bool set = false;
    bool reset = true;
    string color = T_GREEN;
    if(type == "WARNING" || type == "WARN" || type == "FIXME")
        color = T_YELLOW;
    else if(type == "DEBUG")
        color = T_CYAN;
    else if(type == "ERROR" || type == "FAIL" || type == "FATAL" || type == "ASSERT_FAILURE")
        color = T_RED;
    else if(type == "INFO" | type == "EXPECT_FAILURE")
        color = T_MAGENTA;
    else if(type == "LOG")
        color = T_BLUE;

    for(unsigned int i = 0; i < msg.size(); ++i)
    {
        if('`' == msg[i])
        {
            if(!set and reset) 
            {
                set = true;
                reset = false;
                ss << color;
            }
            else if(set && !reset)
            {
                reset = true;
                set = false;
                ss << T_RESET;
            }
        }
        else if('\n' == msg[i])
            ss << "\n + ";
        else
            ss << msg[i];
    }

    /*  Be safe than sorry */
    if(!reset)
        ss << T_RESET;
    cerr << ss.str() << endl;
}

/*-----------------------------------------------------------------------------
 *  Log to a file, and also to console.
 *-----------------------------------------------------------------------------*/
bool isBackTick(char a)
{
    if('`' == a)
        return true;
    return false;
}

void log(string msg, string type, bool redirectToConsole, bool removeTicks)
{
    if(redirectToConsole)
        dump(msg, type, true);

    /* remove any backtick from the string. */
    if(removeTicks)
        remove_if(msg.begin(), msg.end(), isBackTick);

    fstream logF;
    logF.open("__simple__.log", ios::app);

    time_t rawtime; time(&rawtime);
    struct tm* timeinfo;
    timeinfo = localtime(&rawtime);

    logF << asctime(timeinfo) << ": " << msg;

    logF.close();
}


#endif   /* ----- #ifndef print_function_INC  ----- */
