#!/bin/bash
set +e

# Set pythonpath
# If first argument is not "d" then normal execution else run inside python
# debugger.

version=1
if [[ $version -eq 1 ]]; then
    if [[ `uname -a` != *Ubuntu* ]];then
        echo "Using async branch"
        export PYTHONPATH=../../moose_async_git/python
    else
        echo "Using buildq branch"
        export PYTHONPATH=../../moose-github/python
    fi
else
    echo "Using buildq branch"
    export PYTHONPATH=../../moose_svn1.8/moose/branches/async13/python/
fi

function runCode 
{
  $PYC main.py \
    --nml $nmlFile \
    --mumbl $projDir/mumbl.xml \
    --config $projDir/config.xml 
  #twopi -Teps graphs/moose.dot > graphs/topology.eps
}

function testPythonCode 
{
    pf=$1
    echo "== $0 Checking .... $pf"
    pylint -E $p

}

projDir=./models/NeuroConstruct/two_cells
projName=two_cells.nml
nmlFile=$projDir/$projName
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

dotFile=./figs/topology.dot 
if [ -f $dotFile ]; then
    epsFile=${dotFile%.dot}.eps
    echo "== $0" "Converting $dotFile -> $epsFile"
    dot -Teps "$dotFile" > "$epsFile"
fi
