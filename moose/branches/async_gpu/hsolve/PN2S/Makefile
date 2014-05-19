#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment,
#** also known as GENESIS 3 base code.
#**           copyright (C) 2004 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/
#TODO: imported from main Makefile for test, after we can remove it
ifndef CXXFLAGS 
	CXXFLAGS = -g -fpermissive -fno-strict-aliasing -fPIC -fno-inline-functions \
	-Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER -DSVN_REVISION=\"5340M\" \
	-DLINUX -DUSE_GSL -DUSE_HDF5  -DH5_NO_DEPRECATED_SYMBOLS -I/usr/local/hdf5/include 
endif
####################################################################

TARGET = _pn2s.o

COBJS += PN2S_Device.o \
	PN2S_DeviceManager.o \
		 
OBJS += PN2S_Manager.o 

SUBDIR = core
	
MODULES = core/_core.o 

.PHONY: all
all: $(TARGET)
default: all

PN2S_Manager.o: PN2S_Manager.cpp PN2S_Manager.h 
PN2S_Device.o: PN2S_Device.cpp PN2S_Device.h
PN2S_DeviceManager.o: PN2S_DeviceManager.cpp PN2S_DeviceManager.h 

$(TARGET): $(OBJS) $(COBJS) $(SUBDIR) 
#	@(for i in $(SUBDIR) ; do $(MAKE) -C $$i; done)
	$(LD) -r -o  $(TARGET) $(OBJS) $(COBJS) $(MODULES)
	 
$(OBJS):
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I/usr/local/cuda/include -I. -I../basecode -I../msg $< -c 
	
$(COBJS):
	nvcc --compile -o  "$@" "$<" -G -O0 -Xcompiler -fopenmp -Xcompiler -fPIC -g -gencode arch=compute_30,code=sm_30  -x cu     

.PHONY: clean
$(SUBDIR): force
	@(for i in $(SUBDIR) ; do $(MAKE) -C $$i; done)

.PHONY: force
force :;

test: $(TARGET) 
	$(CXX)  PN2S_Test.cpp $(TARGET) -o PN2S_Test -L/usr/local/cuda/lib64 -lcuda -lcudart -lcublas -I/usr/local/cuda/include $(CXXFLAGS)

clean:
	@(for i in $(SUBDIR) ; do $(MAKE) -C $$i clean;  done)
	rm -rf $(OBJS) $(COBJS) $(TARGET) PN2S_Test
