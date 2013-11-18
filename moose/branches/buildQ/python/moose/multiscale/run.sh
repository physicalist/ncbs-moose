#!/bin/bash

set +e
runCode() {
  python2.7 main.py \
    --nml ./models/neuroml/v1.8/Cerebellum.xml \
    --mumbl ./models/mumbl.xml
  #twopi -Teps graphs/moose.dot > graphs/topology.eps
}

runCode
