#!/bin/bash
echo "Updaing async13 branch"
( cd ../async13/ && svn up )
if [ $# -lt 2]; then
    echo "USAGE: ./$0 rev_no"
    exit
fi
svn merge -c $1 ../async13
