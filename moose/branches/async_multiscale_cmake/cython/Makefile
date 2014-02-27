PYC=python
CXX=g++
CC=g++
LDFLAGS=-L/usr/lib -L/usr/local/lib -L/usr/lib/mpi/gcc/openmpi/lib
export LDFLAGS
OBJS = ./pymoose.pyx ./basecode_Id.pxd  \
       ./biophysics_Compartment.pxd \
       ./cython_header.hpp

all : moose_cython.so 
	./test.py

moose_cython.so : $(OBJS)
	$(PYC) ./setup.py build_ext --inplace 

svn : 
	svn add *.pyx *.pxd *.py Makefile run.sh

.PHONY: svn
