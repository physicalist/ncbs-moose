#pragma once

#include "../../headers.h"
#include "Neuron.h"

namespace pn2s
{
namespace models
{

class NeuronVector
{
	vector<Neuron> neurons;
public:
	NeuronVector() {}
	virtual ~NeuronVector();

	Neuron* Create(int gid);
//	typename Neuron::itr Create(int gid);
	Neuron &operator[](int i);
};

}
}
