/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2004 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HEADER_H
#define _HEADER_H

/* Some of these headers are from boost library */
#include "../external/debug/current_function.hpp"
#include "../external/debug/assert.hpp"
#include "../external/debug/static_assert.hpp"

#include "../external/debug/print_function.h"



/*-----------------------------------------------------------------------------
 *  This macro prints the output of a test function onto console. It also keep
 *  track of index of the current test. The index of test is automatically
 *  computed by increamenting the counter.
 *-----------------------------------------------------------------------------*/
extern unsigned int totalTests;
#define tbegin cerr << endl << "Test(" << totalTests << "): " << BOOST_CURRENT_FUNCTION;
#define tend totalTests++; \
    cerr << std::right <<  setw(20) << "test of " << BOOST_CURRENT_FUNCTION << " finished."; 

#include <math.h>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <typeinfo> // used in Conv.h to extract compiler independent typeid
#include <climits> // Required for g++ 4.3.2
#include <cstring> // Required for g++ 4.3.2
#include <cstdlib> // Required for g++ 4.3.2

// Used for INT_MAX and UINT_MAX, but may be done within the compiler
// #include <limits.h>
//
#include <cassert>

using namespace std;
using namespace boost;

// MOOSE version is hard coded here. Can be overridden from a
// makefile.
#ifndef MOOSE_VERSION
#define MOOSE_VERSION "2.0.0"
#endif
// SVN revision number should be automatically detected in top level
// Makefile and passed to gcc. For release versions, it defaults to
// "0".
#ifndef SVN_REVISION
#define SVN_REVISION "0"
#endif
/**
 * Looks up and uniquely identifies functions, on a per-Cinfo basis.
 * These are NOT global indices to identify the function.
 */
typedef unsigned int FuncId;

/** 
 * Looks up data entries.
 */
typedef unsigned int DataId;

/**
 * Identifies data entry on an Element. This is a global index,
 * in that it does not refer to the array on any given node, but uniquely
 * identifies the entry over the entire multinode simulation.
 */
extern const unsigned int ALLDATA; // Defined in consts.cpp

/// Identifies bad DataIndex or FieldIndex in ObjId.
extern const unsigned int BADINDEX; // Defined in consts.cpp

/**
 * Index into Element::vector< vector< MsgFuncBinding > > msgBinding_;
 */
typedef unsigned short BindIndex;

extern const double PI;	// Defined in consts.cpp
extern const double NA; // Defined in consts.cpp
extern const double FaradayConst; // Defined in consts.cpp

class Element;
class Eref;
class OpFunc;
class Cinfo;
class SetGet;
class FuncBarrier;
class ObjId;
class SrcFinfo;

#include "doubleEq.h"
#include "Id.h"
#include "ObjId.h"
#include "Finfo.h"
#include "DestFinfo.h"
#include "ProcInfo.h"
#include "Cinfo.h"
#include "MsgFuncBinding.h"
#include "../msg/Msg.h"
#include "Dinfo.h"
#include "MsgDigest.h"
#include "Element.h"
#include "DataElement.h"
#include "GlobalDataElement.h"
#include "LocalDataElement.h"
#include "Eref.h"
#include "Conv.h"
#include "SrcFinfo.h"

extern DestFinfo* receiveGet();
class Neutral;
#include "OpFuncBase.h"
#include "HopFunc.h"
#include "SetGet.h"
#include "OpFunc.h"
#include "EpFunc.h"
#include "ProcOpFunc.h"
#include "ValueFinfo.h"
#include "LookupValueFinfo.h"
#include "SharedFinfo.h"
#include "FieldElementFinfo.h"
#include "FieldElement.h"
#include "../shell/Neutral.h"

#endif // _HEADER_H
