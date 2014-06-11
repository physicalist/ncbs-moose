#pragma once

namespace pn2s {
namespace models {

class ModelStatistic {
public:
	double dt;
	int nCompts;
	int nModels;
	ModelStatistic(): nCompts(0), nModels(0), dt(1){}
	ModelStatistic(double _dt, int _nModel, int _nCompt): dt(_dt), nModels(_nModel), nCompts(_nCompt){}
	virtual ~ModelStatistic(){}
};

}
}
