#!/bin/bash
set +e

# If first argument is not "d" then normal execution else run inside python
# debugger.

PYC=python2.7
if [ "$1" == "d" ]; then
  PYC=pydb
else
  PYC=python2.7
fi

# Function which run the main script.
runCode() {
  $PYC main.py \
    --nml ./models/neuroml/v1.8/Cerebellum.xml \
    --mumbl ./models/mumbl.xml
  #twopi -Teps graphs/moose.dot > graphs/topology.eps
}

runCode
