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
#define UNIX

#ifndef PI
#define PI 3.141592653589793
#endif

/// Here we set up an enhanced variant of assert, used in unit tests.
#ifndef NDEBUG
# define ASSERT( isOK, message ) \
	if ( !(isOK) ) { \
   cout << "\nERROR: Assert '" << #isOK << "' failed on line " << __LINE__ << "\nin file " << __FILE__ << ": " << #message << endl; \
    exit( 1 ); \
} else { \
	   	cout << "."; \
}
#else
# define ASSERT( unused, message ) do {} while ( false )
#endif

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <cassert>

using namespace std;

class Element;
class Conn;
class Finfo;

// This is here because parallel messaging needs a way to
// access PostMaster buffer from within all the templated
// Ftypes. Ugly.
extern void* getParBuf( const Conn& c, unsigned int size );
extern void* getAsyncParBuf( const Conn& c, unsigned int size );

// Another ugly global, this one for accessing the ids.
class IdManager;
// extern IdManager* idManager();

#include "RecvFunc.h"
#include "Conn.h"
#include "Ftype.h"
#include "FunctionData.h"
#include "Finfo.h"
#include "Id.h"
#include "Element.h"


#define MAX_COMMAND_ARGUMENTS		25
#define MAX_COMMAND_SIZE		1024
#define MAX_MPI_PROCESSES		1024
#define MAX_MPI_RECV_RECORD_SIZE	500
#define OBJECTS_PER_NODE		2

#define MAX_MPI_BUFF_SIZE 		512
#define MAX_MPI_SEND_TIME 		50 

#define	VISUALIZATION_TAG		1
#define MOOSE_COMMAND_ACK_TAG		2
#define SPIKE_TAG			3

#endif // _HEADER_H
