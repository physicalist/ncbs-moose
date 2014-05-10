///////////////////////////////////////////////////////////
//  PN2SModel_Compartment.h
//  Implementation of the Class PN2SModel_Compartment
//  Created on:      27-Dec-2013 9:35:18 PM
//  Original author: saeed
///////////////////////////////////////////////////////////

#if !defined(EA_F11DAA80_6F7A_4ad9_B555_12F0C681E799__INCLUDED_)
#define EA_F11DAA80_6F7A_4ad9_B555_12F0C681E799__INCLUDED_

#include "../Definitions.h"
#include "PN2SModel_CaChannel.h"
#include "PN2SModel_CustomChannel.h"
#include "PN2SModel_HHChannel.h"
#include "PN2SModel_SynapticChannel.h"

class PN2SModel_Compartment
{
public:
	vector< unsigned int > children;	///< Hines indices of child compts
	double Ra;
	double Rm;
	double Cm;
	double Em;
	double Vm;
	double initVm;

	vector<PN2SModel_HHChannel> hhchannels;

	PN2SModel_Compartment();
	PN2SModel_Compartment(uint _id);
	virtual ~PN2SModel_Compartment();

	//Copy constractor
	PN2SModel_Compartment( const PN2SModel_Compartment& other );
private:

};
#endif // !defined(EA_F11DAA80_6F7A_4ad9_B555_12F0C681E799__INCLUDED_)
