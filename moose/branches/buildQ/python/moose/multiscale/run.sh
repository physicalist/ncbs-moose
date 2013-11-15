#!/bin/bash

set +e
runCode() {
  python main.py \
    --nml ./models/neuroml/v1.8/Cerebellum.xml \
    --adaptor ./schema/adaptor/adaptor.xml \
    --mumbl ./schema/mumbl/mumbl.xml
  twopi -Teps graphs/moose.dot > graphs/topology.eps
}

runCode
