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
#include <stdexcept>
#include <sstream>
#include <typeinfo> // used in Conv.h to extract compiler independent typeid
#include <climits> // Required for g++ 4.3.2
#include <cstring> // Required for g++ 4.3.2
#include <cstdlib> // Required for g++ 4.3.2

#include "declarations.h"

// Used for INT_MAX and UINT_MAX, but may be done within the compiler
// #include <limits.h>
//
#include <cassert>

using namespace std;
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
#include "../external/debug/print_function.h"

#endif // _HEADER_H
