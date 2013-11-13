#!/bin/bash

runCode() {
  python main.py \
    --nml ./models/neuroml/v1.8/Cerebellum.xml \
    --adaptor ./schema/adaptor/adaptor.xml \
    --mumbl ./schema/mumbl/mumbl.xml
  neato -Teps graphs/moose.dot > graphs/moose.eps
  dot -Teps graphs/moose.dot > graphs/topology.eps
}

runCode
