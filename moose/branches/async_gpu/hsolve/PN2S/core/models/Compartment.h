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
//#include "HHChannel.h"
//#include "SynapticChannel.h"

namespace pn2s
{
namespace models
{

class Compartment
{
public:
	typedef typename std::vector<Compartment>::iterator itr;

	unsigned int gid;
//	ChannelVec& compt;
	vector< unsigned int > children;	///< Hines indices of child compts

	Compartment(unsigned int _gid): gid(_gid){
		children.resize(2);
	}
	virtual ~Compartment(){}

	//Copy constractor is necessary because at Vector assign, information will copy through it.
	Compartment( const Compartment& other );
	Compartment& operator=(Compartment arg){}

};

}
}
#endif // !defined(EA_F11DAA80_6F7A_4ad9_B555_12F0C681E799__INCLUDED_)
