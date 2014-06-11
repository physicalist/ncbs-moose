///////////////////////////////////////////////////////////
//  Compartment.h
//  Implementation of the Class Compartment
//  Created on:      27-Dec-2013 9:35:18 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#if !defined(EA_F11DAA80_6F7A_4ad9_B555_12F0C681E799__INCLUDED_)
#define EA_F11DAA80_6F7A_4ad9_B555_12F0C681E799__INCLUDED_

#include "../../headers.h"
//#include "CaChannel.h"
//#include "CustomChannel.h"
#include "HHChannel.h"
//#include "SynapticChannel.h"

namespace pn2s
{
namespace models
{

class Compartment
{
	friend class SolverComps;
	int _index;
public:
	int address;
	int gid;

	vector< unsigned int > children;	///< Hines indices of child compts

//	vector<HHChannel > hhchannels;

	Compartment(int);
	virtual ~Compartment();

	//Copy constractor is necessary because at Vector assign, information will copy through it.
	Compartment( const Compartment& other );

};

}
}
#endif // !defined(EA_F11DAA80_6F7A_4ad9_B555_12F0C681E799__INCLUDED_)
