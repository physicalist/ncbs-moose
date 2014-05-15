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

TARGET = _models.o

		 
OBJS +=		PN2SModel_HHChannel.o \
		PN2SModel.o \
		PN2SModel_Compartment.o 

.PHONY: all
all: $(TARGET)
default: all

PN2SModel_HHChannel.o: PN2SModel_HHChannel.cpp PN2SModel_HHChannel.h
PN2SModel.o: PN2SModel.cpp PN2SModel.h 
PN2SModel_Compartment.o: PN2SModel_Compartment.cpp PN2SModel_Compartment.h

$(TARGET): $(OBJS) $(COBJS)
	$(LD) -r -o  $(TARGET) $(OBJS) $(COBJS) 
$(OBJS):
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I/usr/local/cuda/include -I. -I../basecode -I../msg $< -c 
	
.PHONY: clean
clean:
	rm -rf $(OBJS) $(COBJS) $(TARGET)
