#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2004 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/
# $Id: $
#
# $Log: $
#

TARGET = hsolve1.o

OBJ = \
	HSolveComputeMus.o \
	HSolveModelMus.o

HEADERS = \
	../../basecode/header.h \
	../../basecode/Conn.h \
	../../basecode/Cinfo.h \
	../../basecode/Ftype.h \
	../../basecode/Field.h \
	../../basecode/Finfo.h \
	../../basecode/MsgSrc.h \
	../../basecode/RelayFinfo.h \
	../../basecode/ValueFinfo.h \
	../../basecode/DestFinfo.h \
	../../basecode/SingleSrcFinfo.h \
	../../basecode/NSrcFinfo.h \
	../../basecode/ReturnFinfo.h \
	../../basecode/SharedFinfo.h \
	../../basecode/ObjFinfo.h \
	../../basecode/Element.h \
	../../basecode/Neutral.h \
	../../basecode/ProcInfo.h \
	

default: $(TARGET)

$(OBJ)	: $(HEADERS)
HSolveComputeMus.o: \
	../HSolveModel.h \
	../HSolveCompute.h \
	HSolveModelMus.h \
	HSolveComputeMus.h \
	../../biophysics/Compartment.h \
	../../biophysics/CompartmentWrapper.h
HSolveModelMus.o: ../HSolveModel.h HSolveModelMus.h

.cpp.o:
	$(CXX) $(CFLAGS) -I../../basecode $< -c

$(TARGET):		$(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(LIBS)

clean:
	-rm -f *.o $(TARGET) core core.*
