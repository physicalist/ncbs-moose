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

TARGET = basecode.o

#  CXXFLAGS += -I.. -I../kinetics -I../biophysics

OBJ = \
	Element.o	\
	Eref.o	\
	Finfo.o	\
	Cinfo.o	\
	Msg.o	\
	Conn.o \
	SrcFinfo.o \
	Id.o \
	testSync.o	\
	testAsync.o	\
	Neutral.o	\
	Shell.o	\
	Qinfo.o \
	DataId.o \
	Message.o \
	SetGet.o \
	SparseMatrix.o \
	SparseMsg.o \
	main.o	\

HEADERS = \
	header.h \
	Cinfo.h \
	Conn.h \
	Conv.h \
	Data.h \
	DataId.h \
	Dinfo.h \
	Element.h \
	Eref.h \
	Finfo.h \
	Id.h \
	Msg.h \
	Neutral.h \
	OpFunc.h \
	EpFunc.h \
	ProcInfo.h \
	Qinfo.h \
	SrcFinfo.h \
	ValueFinfo.h \
	Shell.h \

default: $(TARGET)

$(OBJ)	: $(HEADERS)
testAsync.o:	SparseMatrix.h SetGet.h
SparseMsg.o:	SparseMatrix.h
#TestBasecode.o:	Shell.h ShellWrapper.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -c

$(TARGET):		$(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ)

clean:
	-rm -f *.o $(TARGET) core core.*
