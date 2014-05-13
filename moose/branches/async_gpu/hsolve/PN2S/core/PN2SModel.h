///////////////////////////////////////////////////////////
//  PN2SModel_Neutral.h
//  Implementation of the Class PN2SModel_Neutral
//  Created on:      26-Dec-2013 9:48:37 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A33409E93_3943_416a_AEEF_DDB798B0DF86__INCLUDED_)
#define A33409E93_3943_416a_AEEF_DDB798B0DF86__INCLUDED_

#include "../Definitions.h"
#include "PN2SModel_HHChannel.h"
#include "PN2SModel_Compartment.h"

/*
 *
 */
class PN2SModel
{
public:
	uint id;

	vector<PN2SModel_HHChannel> hhChannels;
	vector<PN2SModel_Compartment> compts;

	PN2SModel(uint _id);
	virtual ~PN2SModel();
};
#endif // !defined(A33409E93_3943_416a_AEEF_DDB798B0DF86__INCLUDED_)
