///////////////////////////////////////////////////////////
//  Axon.h
//  Implementation of the Class Axon
//  Created on:      26-Dec-2013 4:20:50 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#if !defined(EA_0D7CEB44_7D8D_4cb4_92A0_799715709F94__INCLUDED_)
#define EA_0D7CEB44_7D8D_4cb4_92A0_799715709F94__INCLUDED_

#include "SynapticChannel.h"

namespace pn2s
{
namespace models
{

class Axon
{

public:
	Axon();
	virtual ~Axon();
	SynapticChannel *m_SynapticChannel;

};

}
}
#endif // !defined(EA_0D7CEB44_7D8D_4cb4_92A0_799715709F94__INCLUDED_)
