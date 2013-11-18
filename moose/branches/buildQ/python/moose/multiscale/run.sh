#!/bin/bash

set +e
runCode() {
  python main.py \
    --nml ./models/neuroml/v1.8/Cerebellum.xml \
    --mumbl ./models/mumbl.xml
  #twopi -Teps graphs/moose.dot > graphs/topology.eps
}

runCode
