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
	CXXFLAGS =  -g -fpermissive -fno-strict-aliasing -fPIC -fno-inline-functions \
	-Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER -DSVN_REVISION=\"5340M\" \
	-DLINUX -DUSE_GSL -DUSE_HDF5  -DH5_NO_DEPRECATED_SYMBOLS -I/usr/local/hdf5/include 
endif

ifndef CUDAFLAGS 
	CUDAFLAGS = -G -O0 -Xcompiler -fopenmp -Xcompiler -fPIC -g -gencode arch=compute_30,code=compute_30 -gencode arch=compute_30,code=sm_30 -x cu  -I./libs -I./libs/tbb/include
endif

####################################################################

TARGET = _models.o
HEADERS = ../../headers.h
		 
OBJS +=		HHChannel.o \
			Model.o \
			Compartment.o \
#			Matrix.o	\
			

COBJS += SolverChannels.o	\
		SolverComps.o	\
		PField.o		\
		solve.o	\

.PHONY: all
all: $(TARGET)
default: all

Model.o: Model.cpp Model.h HHChannel.cpp HHChannel.h Compartment.cpp Compartment.h Matrix.cpp Matrix.h 
HHChannel.o: HHChannel.cpp HHChannel.h $
Compartment.o: Compartment.cpp Compartment.h 
CompartmentVector.o: CompartmentVector.cpp CompartmentVector.h 
Matrix.o: Matrix.cpp Matrix.h 
Neuron.o: Neuron.cpp Neuron.h Compartment.o
NeuronVector.o: NeuronVector.cpp NeuronVector.h
PField.o: PField.cpp PField.h
SolverChannels.o: SolverChannels.cpp SolverChannels.h 
SolverComps.o: SolverComps.cu SolverComps.h
solve.o: solve.cu solve.h operations.h

$(TARGET): $(OBJS) $(COBJS) 
	$(LD) -r -o  $(TARGET) $(OBJS) $(COBJS) 
$(OBJS): 
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I/usr/local/cuda/include -I. -I../basecode -I../msg -I../../libs $< -c 	
$(COBJS): 
	nvcc --compile $(CUDAFLAGS) -o  "$@" "$<"
	
.PHONY: clean
clean:
	rm -rf $(OBJS) $(COBJS) $(TARGET)	
