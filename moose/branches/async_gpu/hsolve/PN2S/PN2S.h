///////////////////////////////////////////////////////////
//  PN2S.h
//  Contains definitions and required header includes
//
//  Created on:      26-Dec-2013 4:18:10 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A412A01E5_7D8D_4c56_A915_73B69DCFE454__INCLUDED_)
#define A412A01E5_7D8D_4c56_A915_73B69DCFE454__INCLUDED_

#include <math.h>
#include <algorithm>
#include <string>

#include <vector>
#include <map>
#include <set>
#include <deque>

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "PN2S_HelperFunctions.h"

//#include <typeinfo> // used in Conv.h to extract compiler independent typeid
//#include <climits> // Required for g++ 4.3.2
//#include <cstring> // Required for g++ 4.3.2
//#include <cstdlib> // Required for g++ 4.3.2
using namespace std;

//	Architectures
#define ARCH_SM13       (0)
#define ARCH_SM20       (1)
#define ARCH_SM30       (2)
#define ARCH_SM35       (3)

#define CURRENT_ARCH 	ARCH_SM30
#define CURRENT_TYPE	double
//#define checkCudaErrors(val)    ( (val), #val, __FILE__, __LINE__ )

//#define hsc_uint uint
#define hscID_t uint


#endif // !defined(A412A01E5_7D8D_4c56_A915_73B69DCFE454__INCLUDED_)
