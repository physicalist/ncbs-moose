#!/bin/bash
set +e

# Set pythonpath
# If first argument is not "d" then normal execution else run inside python
# debugger.

export PYTHONPATH=../../moose-github/python
#export PYTHONPATH=../../moose_svn1.7/moose/branches/async13/python/
function runCode 
{
  $PYC main.py \
    --nml ./models/simple_cell/SimpleNeuroML.xml \
    --mumbl ./models/simple_cell/mumbl.xml \
    --config ./models/simple_cell/config.xml 
  #twopi -Teps graphs/moose.dot > graphs/topology.eps
}

function testPythonCode 
{
    pf=$1
    echo "Checking .... $pf"
    pylint -E $pf
}

PYC=python2.7
if [ "$1" == "d" ]; then
  PYC="gdb -ex r --args python2.7"
  runCode
elif [ "$1" == "c" ]; then 
    FILES=$(find . -name "*.py" -type f)
    for pf in $FILES; do
        testPythonCode $pf
    done
else
  PYC=python2.7
  runCode
fi

