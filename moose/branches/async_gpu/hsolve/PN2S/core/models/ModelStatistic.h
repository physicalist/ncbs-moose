#pragma once

namespace pn2s {
namespace models {

class ModelStatistic {
public:
	double dt;
	size_t nCompts;
	size_t nModels;
	ModelStatistic(): nCompts(0), nModels(0), dt(1){}
	ModelStatistic(double _dt, size_t _nModel, size_t _nCompt): dt(_dt), nModels(_nModel), nCompts(_nCompt){}
	virtual ~ModelStatistic(){}
};

}
}
