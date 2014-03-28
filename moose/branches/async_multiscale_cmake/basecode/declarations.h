/*
 * =====================================================================================
 *
 *       Filename:  declarations.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Thursday 27 March 2014 08:35:25  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dilawar Singh (), dilawar@ee.iitb.ac.in
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  DECLARATIONS_INC
#define  DECLARATIONS_INC

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
 * Looks up and uniquely identifies Msgs. This is a global index
 */
typedef unsigned int MsgId;

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


#endif   /* ----- #ifndef DECLARATIONS_INC  ----- */
