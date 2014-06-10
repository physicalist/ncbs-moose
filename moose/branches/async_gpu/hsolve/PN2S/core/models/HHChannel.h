///////////////////////////////////////////////////////////
//  HHChannel.h
//  Implementation of the Class HHChannel
//  Created on:      26-Dec-2013 4:20:54 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A904F55B9_7DDF_45c6_81E6_3396EFC0EED4__INCLUDED_)
#define A904F55B9_7DDF_45c6_81E6_3396EFC0EED4__INCLUDED_
#include "../../headers.h"
namespace pn2s
{
namespace models
{

class HHChannel
{
public:
	HHChannel();
	virtual ~HHChannel();

	TYPE_ Gbar;
	TYPE_ Ek;
	TYPE_ Gk;
	TYPE_ Ik;
	TYPE_ Vm;
};

}
}
#endif // !defined(A904F55B9_7DDF_45c6_81E6_3396EFC0EED4__INCLUDED_)
