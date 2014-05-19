///////////////////////////////////////////////////////////
//  PN2SModel_Neutral.h
//  Implementation of the Class PN2SModel_Neutral
//  Created on:      26-Dec-2013 9:48:37 PM
//  Original author: Saeed Shariati
///////////////////////////////////////////////////////////

#if !defined(A33409E93_3943_416a_AEEF_DDB798B0DF86__INCLUDED_)
#define A33409E93_3943_416a_AEEF_DDB798B0DF86__INCLUDED_

#include "../../PN2S.h"
#include "PN2SModel_HHChannel.h"
#include "PN2SModel_Compartment.h"

/*
 *
 */
template <typename T, int arch>
class PN2SModel
{
public:
	uint id;

	vector<PN2SModel_HHChannel<T,arch> > hhChannels;
	vector<PN2SModel_Compartment<T,arch> > compts;

	PN2SModel(uint _id);
	virtual ~PN2SModel();
};
#endif // !defined(A33409E93_3943_416a_AEEF_DDB798B0DF86__INCLUDED_)
