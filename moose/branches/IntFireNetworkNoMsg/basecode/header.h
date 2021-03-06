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

#include <math.h>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <climits> // Required for g++ 4.3.2
#include <cstring> // Required for g++ 4.3.2
#include <cstdlib> // Required for g++ 4.3.2


// Used for INT_MAX and UINT_MAX, but may be done within the compiler
// #include <limits.h>
//
#include <cassert>

using namespace std;

/**
 * Looks up and uniquely identifies functions.
 */
typedef unsigned int FuncId;

/**
 * Looks up Synchronous messages on elements
 */
typedef unsigned int SyncId;

/**
 * Looks up data entries on elements
 */
extern const FuncId ENDFUNC;

class Element;
class Eref;
class OpFunc;
class Id;
class Qinfo;
class Data;

#include "DataId.h"
#include "Finfo.h"
#include "ProcInfo.h"
#include "Cinfo.h"
#include "Data.h"
#include "Msg.h"
#include "Qinfo.h"
#include "Conn.h"
#include "Element.h"
#include "Eref.h"
#include "Id.h"
#include "Conv.h"
#include "SrcFinfo.h"
#include "Shell.h"
#include "SetGet.h"
#include "OpFunc.h"
#include "EpFunc.h"
#include "ValueFinfo.h"

#endif // _HEADER_H
