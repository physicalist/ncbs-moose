#ifndef MOOSE_SIMULATION_PARAMETERS
#define MOOSE_SIMULATION_PARAMETERS

#include "charm++.h"

struct SimulationParameters {
  double dt;

  void pup(PUP::er &p){
    p|dt;
  }
};

#endif // MOOSE_SIMULATION_PARAMETERS

