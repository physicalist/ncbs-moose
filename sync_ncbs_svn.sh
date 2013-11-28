#!/bin/bash 
thisDir=$(pwd)
echo "updating svn"
(
    cd ../../NCBS/moose-code/moose/branches/buildQ/ 
    svn update
    rsync -avC . $thisDir 
)

