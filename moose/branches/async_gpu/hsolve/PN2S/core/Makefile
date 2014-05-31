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

TARGET = _core.o

COBJS += 	ModelPack.o	\

		 
OBJS += NetworkAnalyzer.o 
		
SUBDIR = 	models \
			solvers
	
MODULES = 	models/_models.o \
			solvers/_solvers.o 
		
.PHONY: all
all: $(TARGET)
default: all

ModelPack.o: ModelPack.cpp ModelPack.h solvers/SolverChannels.cpp solvers/SolverChannels.h solvers/SolverComps.cu solvers/SolverComps.h 
NetworkAnalyzer.o: NetworkAnalyzer.cpp NetworkAnalyzer.h

$(TARGET): $(OBJS) $(COBJS) $(SUBDIR) 
	$(LD) -r -o  $(TARGET) $(OBJS) $(COBJS) $(MODULES)
	 
$(OBJS):
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I/usr/local/cuda/include -I. -I../basecode -I../msg -I../extra $< -c 
	
$(COBJS):
	nvcc --compile -o  "$@" "$<" -G -O0 -Xcompiler -fopenmp -Xcompiler -fPIC -g -gencode arch=compute_30,code=sm_30  -x cu  -I../extra   

.PHONY: clean
$(SUBDIR): force
	@(for i in $(SUBDIR) ; do $(MAKE) -C $$i; done)

.PHONY: force
force :;

clean:
	@(for i in $(SUBDIR) ; do $(MAKE) -C $$i clean;  done)
	rm -rf $(OBJS) $(COBJS) $(TARGET)
