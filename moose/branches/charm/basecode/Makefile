#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment,
#** also known as GENESIS 3 base code.
#**           copyright (C) 2004 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/
# $Id: $
#
# $Log: $
#

TARGET = _basecode.o


#  CXXFLAGS += -I.. -I../kinetics -I../biophysics

INCPATH = -I../msg

OBJ = \
	consts.o	\
	Element.o	\
	Qinfo.o \
	DataHandler.o \
	ZeroDimHandler.o \
	ZeroDimParallelHandler.o \
	BlockHandler.o \
	OneDimHandler.o \
	TwoDimHandler.o \
	AnyDimHandler.o \
	DataHandlerWrapper.o \
	FieldDataHandlerBase.o \
	MsgDataHandler.o \
	ZombieHandler.o \
	Eref.o	\
	Finfo.o	\
	CcsFinfo.o \
	DestFinfo.o \
	Cinfo.o	\
	CcsCinfo.o \
	SrcFinfo.o \
	ValueFinfo.o \
	SharedFinfo.o \
	FieldElementFinfo.o \
	Id.o \
	ObjId.o \
	DataId.o \
	SetGet.o \
	OpFunc.o \
	FieldOpFunc.o \
	SparseMatrix.o \
	PrepackedBuffer.o \
	FuncBarrier.o \
	doubleEq.o \
	ReduceBase.o \
	testAsync.o	\
	main.o	\

ifeq ($(BUILD),charm++)
OBJ += SetGetRegistration.o \
       DefSetGetCcsHandlerStrings.o \
       SetGetCcsClient.o \
       CcsPackUnpack.o \
       CcsId.o \
       CcsObjId.o
endif

PARSER_OBJS = CcsId_parser.o \
              CcsObjId_parser.o \
              sundry.o
# sundry.o is required for pymoose: defines some basic functions 
# that are referenced in pup framework. since we link pymoose
# with g++ instead of charm++, we have to provide our own 
# definitions for these functions. 

HEADERS = \
	header.h \
	Cinfo.h \
	PrepackedBuffer.h \
	Conv.h \
	DataId.h \
	Dinfo.h \
	DataDimensions.h \
	DataHandler.h \
	BlockHandler.h \
	AnyDimHandler.h \
	ZeroDimHandler.h \
	ZeroDimParallelHandler.h \
	OneDimHandler.h \
	FieldDataHandlerBase.h \
	FieldDataHandler.h \
	MsgDataHandler.h \
	ZombieHandler.h \
	Element.h \
	Eref.h \
	Finfo.h \
	DestFinfo.h \
	Id.h \
	ObjId.h \
	../msg/Msg.h \
	OpFuncBase.h \
	OpFunc.h \
	EpFunc.h \
	ProcInfo.h \
	Qinfo.h \
	SrcFinfo.h \
	ValueFinfo.h \
	LookupValueFinfo.h \
	SharedFinfo.h \
	FieldElementFinfo.h \
	MsgFuncBinding.h \
	ReduceBase.h \
	ReduceFinfo.h \

default: $(TARGET) $(PARSER_OBJS)

$(OBJ)	: $(HEADERS) ../shell/Shell.h
Element.o:	DataHandlerWrapper.h
testAsync.o:	TestAsync.h SparseMatrix.h SetGet.h ../scheduling/Clock.h ../scheduling/Tick.h ../scheduling/TickPtr.h ../scheduling/TickMgr.h ../biophysics/IntFire.h ../biophysics/SynBase.h
SparseMsg.o:	SparseMatrix.h
SetGet.o:	SetGet.h ../shell/Neutral.h
DataHandlerWrapper.o: DataHandlerWrapper.h
FieldDataHandlerBase.o: FieldOpFunc.h
FieldOpFunc.o: FieldOpFunc.h FieldDataHandlerBase.h
FuncBarrier.o:	FuncBarrier.h

CcsId_parser.o: CcsId.h
CcsObjId_parser.o: CcsObjId.h


ifeq ($(BUILD), charm++)
SetGetRegistration.o: SetGetRegistration.h SetGetCcsServer.h SetGetCcsClient.h SetGetWrapper.h
SetGetCcsClient.o: SetGetWrapper.h
endif

#TestBasecode.o:	Shell.h ShellWrapper.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCPATH) $< -c

.PHONY: pymoose

pymoose: CXXFLAGS += -DPYMOOSE
pymoose: OBJ := $(filter-out main.o,$(OBJ))
pymoose: OBJ += main_pymoose.o
pymoose: _basecode_pymoose.o main_pymoose.o
_basecode_pymoose.o: $(OBJ) main_pymoose.o
	$(LD) -r -o _basecode_pymoose.o $(OBJ)

main_pymoose.o: main.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCPATH) $<
	@echo "Compiled version of main file for pymoose."

$(TARGET):		$(OBJ) $(HEADERS) 
	$(LD) -r -o $(TARGET) $(OBJ)

clean:
	-rm -f *.o $(TARGET) core core.*
