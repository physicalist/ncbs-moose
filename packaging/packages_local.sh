#!/bin/bash
echo "This scripts creates various type of packages of moose to be distributed \
    locally"
cd ./moose_3.0.0/build/
cmake ..
make 
echo "Creating self-extracting package"
cpack -G STGZ
echo "Creating debian package"
cpack -G DEB
echo "Creating RPM package"
cpack -G RPM
