#!/bin/bash

runCode() {
  python main.py \
    --nml ./examples/NML2_FullNeuroML.nml \
    --adaptor ./schema/adaptor/adaptor.xml \
    --mumbl ./schema/mumbl/mumbl.xml
  neato -Teps graphs/moose.dot > graphs/moose.eps
  dot -Teps graphs/moose.dot > graphs/topology.eps
}

runCode
