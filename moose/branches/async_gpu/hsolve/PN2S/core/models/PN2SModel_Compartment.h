///////////////////////////////////////////////////////////
//  PN2SModel_Compartment.h
//  Implementation of the Class PN2SModel_Compartment
//  Created on:      27-Dec-2013 9:35:18 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#if !defined(EA_F11DAA80_6F7A_4ad9_B555_12F0C681E799__INCLUDED_)
#define EA_F11DAA80_6F7A_4ad9_B555_12F0C681E799__INCLUDED_

#include "../../PN2S.h"
#include "PN2SModel_CaChannel.h"
#include "PN2SModel_CustomChannel.h"
#include "PN2SModel_HHChannel.h"
#include "PN2SModel_SynapticChannel.h"

template <typename T, int arch>
class PN2SModel_Compartment
{
public:
	vector< unsigned int > children;	///< Hines indices of child compts
	T Ra;
	T Rm;
	T Cm;
	T Em;
	T Vm;
	T initVm;

	vector<PN2SModel_HHChannel<T,arch> > hhchannels;

	PN2SModel_Compartment();
	PN2SModel_Compartment(uint _id);
	virtual ~PN2SModel_Compartment();

	//Copy constractor
	PN2SModel_Compartment( const PN2SModel_Compartment<T,arch>& other );
private:

};
#endif // !defined(EA_F11DAA80_6F7A_4ad9_B555_12F0C681E799__INCLUDED_)
