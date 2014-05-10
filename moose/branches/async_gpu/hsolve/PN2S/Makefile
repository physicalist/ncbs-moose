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
		 
OBJS += PN2S_Manager.o \
	PN2S_Scheduler.o \

SUBDIR = \
	model \
	solver
	
MODULES = 	model/_model.o \
			solver/_solver.o

.PHONY: all
all: $(TARGET)
default: all

PN2S_Manager.o: PN2S_Manager.cpp PN2S_Manager.h 
PN2S_Scheduler.o: PN2S_Scheduler.cpp PN2S_Scheduler.h 
PN2S_Device.o: PN2S_Device.cpp PN2S_Device.h
PN2S_DeviceManager.o: PN2S_DeviceManager.cpp PN2S_DeviceManager.h 

$(TARGET): $(OBJS) $(COBJS) $(SUBDIR)
#	@(for i in $(SUBDIR) ; do $(MAKE) -C $$i; done)
	$(LD) -r -o  $(TARGET) $(OBJS) $(COBJS) $(MODULES)
	 
$(OBJS):
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I/usr/local/cuda/include -I. -I../basecode -I../msg $< -c 
	
$(COBJS):
	nvcc --compile -o  "$@" "$<" -G -O0 -Xcompiler -fPIC -g -gencode arch=compute_30,code=sm_30  -x cu     

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

#TARGET = _hsc.o
#
#COBJS += PN2S_Solver.o 		\
#		 
#OBJS += 
#		
#TEST_TARGET = PN2S_Test
#TEST_OBJ = PN2S_Test.o
##PROFILE_TARGET = PN2S_prof.out
#CXXFLAGS = -g -O0
#
##HEADERS = \
#	../basecode/header.h
#

#
#default: $(TARGET)
#	/usr/local/cuda-5.5/bin/nvcc -ccbin g++ -G -O0 -g -I./inc  -m64    -gencode arch=compute_10,code=sm_10 -gencode arch=compute_20,code=sm_20 -gencode arch=compute_30,code=sm_30 -gencode arch=compute_35,code=\"sm_35,compute_35\" -o simpleStreams simpleStreams.cu
#	#/usr/local/cuda-5.5/bin/nvcc -I"/usr/local/cuda-5.5/samples/0_Simple" -I"/usr/local/cuda-5.5/samples/common/inc" -I"/src/saeed/cuda-workspace/streams" -G -g -O0 -m64 -gencode arch=compute_30,code=sm_30 -odir "src" -M -o "src/simpleStreams.d" "../src/simpleStreams.cu"
# 	#/usr/local/cuda-5.5/bin/nvcc --compile -G -I"/usr/local/cuda-5.5/samples/0_Simple" -I"/usr/local/cuda-5.5/samples/common/inc" -I"/src/saeed/cuda-workspace/streams" -O0 -g -gencode arch=compute_30,code=compute_30 -gencode arch=compute_30,code=sm_30 -m64  -x cu -o  "src/simpleStreams.o" "../src/simpleStreams.cu"
#	
#	
#$(OBJ)	: $(HEADERS)
#
#.cpp.o:
#	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I. -I../basecode -I../msg $< -c -I/usr/local/cuda/include
#
#$(TARGET): $(SUBDIR) $(TEST_OBJ) $(OBJ) $(SMOLDYN_OBJ) $(HEADERS)  
#	$(LD) -r -o $(TARGET) $(OBJ) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) $(GSL_LIBS) $(CLIB)
#	$(CXX) $(CXXFLAGS) $(TEST_OBJ) $(TARGET) -o $(TEST_TARGET)
#	

#	
##	valgrind --tool=callgrind ./PN2S_test
##	kcachegrind $(ls "callgrind.out.*")
#	
#clean:
#	@(for i in $(SUBDIR) ; do $(MAKE) -C $$i clean;  done)
#	rm -f *.o $(TARGET) $(TEST_TARGET) $(PROFILE_TARGET) gmon.out callgrind.out*
