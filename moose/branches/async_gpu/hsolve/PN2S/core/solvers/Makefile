#/**********************************************************************
# ** This program is part of 'MOOSE', the
# ** Multiscale Object Oriented Simulation Environment.
# **   copyright (C) 2003-2011 Upinder S. Bhalla, Niraj Dudani and NCBS
# ** It is made available under the terms of the
# ** GNU Lesser General Public License version 2.1
# ** See the file COPYING.LIB for the full notice.
# **********************************************************************/

#TODO: imported from main Makefile for test, after we can remove it
ifndef CXXFLAGS 
	CXXFLAGS = -g -fpermissive -fno-strict-aliasing -fPIC -fno-inline-functions \
	-Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER -DSVN_REVISION=\"5340M\" \
	-DLINUX -DUSE_GSL -DUSE_HDF5  -DH5_NO_DEPRECATED_SYMBOLS -I/usr/local/hdf5/include 
endif
####################################################################

TARGET = _solvers.o

COBJS += PN2S_SolverChannels.o	\
		PN2S_SolverComps.o	\
		solve.o	\
		PN2S_Field.o		
		 
#OBJS += 

.PHONY: all
all: $(TARGET)
default: all

PN2S_Field.o: PN2S_Field.cpp PN2S_Field.h
PN2S_SolverChannels.o: PN2S_SolverChannels.cpp PN2S_SolverChannels.h 
PN2S_SolverComps.o: PN2S_SolverComps.cu PN2S_SolverComps.h
solve.o: solve.cu solve.h operations.h

$(TARGET): $(OBJS) $(COBJS)
	$(LD) -r -o  $(TARGET) $(OBJS) $(COBJS) 
$(OBJS):
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I/usr/local/cuda/include -I. -I../basecode -I../msg $< -c 
	
$(COBJS):
	nvcc --compile -o  "$@" "$<" -G -O0 -Xcompiler -fPIC -g -gencode arch=compute_30,code=sm_30 -gencode arch=compute_20,code=sm_20 -x cu -DDO_UNIT_TESTS

.PHONY: clean
clean:
	rm -rf $(OBJS) $(COBJS) $(TARGET)
