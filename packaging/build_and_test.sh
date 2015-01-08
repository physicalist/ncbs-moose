#!/bin/bash
echo "Replacing trunk to moose_3.0.1"
svn status
( cd ./packaging_scripts && ./packages_local_using_cmake.sh )
echo "Always modify trunk and merge to branch"
