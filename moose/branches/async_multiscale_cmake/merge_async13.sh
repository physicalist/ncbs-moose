#!/bin/bash
echo "Updaing async13 branch"
cd .. && svn up && cd async_multiscale_cmake
if [ $# -lt 1 ]; then
    echo "USAGE: ./$0 rev_no"
    exit
fi
echo "Merging $1 of ../async13/ to async_multiscale_cmake"
svn merge -c $1 ../async13
