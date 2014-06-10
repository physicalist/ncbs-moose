#pragma once

#include "../../headers.h"
#include "CompartmentVector.h"
//#include "HHChannel.h"
//#include "Compartment.h"
//#include "Matrix.h"

namespace pn2s
{
namespace models
{


class Neuron
{
	int _compt_base;
	int _compt_size;

public:
	typedef typename std::vector<Neuron>::iterator itr;

	unsigned int gid;

	CompartmentVector& compt();

	Neuron(unsigned int _gid, int idx);
	virtual ~Neuron(){}

	Neuron( const Neuron& other );
	Neuron& operator=(Neuron arg);
};

}
}
