#!/bin/bash
echo "This scripts creates various type of packages of moose to be distributed \
    locally"
mkdir -p  _build
cd _build/
cmake ../moose_3.0.1/
make 
echo "Creating self-extracting package"
cpack -G STGZ
echo "Creating debian package"
cpack -G DEB
echo "Creating RPM package"
cpack -G RPM
