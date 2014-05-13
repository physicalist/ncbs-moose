///////////////////////////////////////////////////////////
//  PN2SModel_Axon.h
//  Implementation of the Class PN2SModel_Axon
//  Created on:      26-Dec-2013 4:20:50 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#if !defined(EA_0D7CEB44_7D8D_4cb4_92A0_799715709F94__INCLUDED_)
#define EA_0D7CEB44_7D8D_4cb4_92A0_799715709F94__INCLUDED_

#include "PN2SModel_SynapticChannel.h"

class PN2SModel_Axon
{

public:
	PN2SModel_Axon();
	virtual ~PN2SModel_Axon();
	PN2SModel_SynapticChannel *m_PN2SModel_SynapticChannel;

};
#endif // !defined(EA_0D7CEB44_7D8D_4cb4_92A0_799715709F94__INCLUDED_)
