#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment,
#** also known as GENESIS 3 base code.
#**           copyright (C) 2004 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = parallel.o

OBJ = \
	PostMaster.o	\
	ProxyElement.o	\
	AsyncDestFinfo.o	\

HEADERS = \
	../basecode/header.h \
	../connections/Conn.h \
	../basecode/Element.h \

default: $(TARGET)

$(OBJ)	: $(HEADERS)
PostMaster.o:	PostMaster.h

ifneq (,$(findstring DUSE_MPI,$(CFLAGS)))
    CXX = mpicxx
else
    CXX = g++
endif

.cpp.o:
	$(CXX) $(CFLAGS) -I../basecode -I.. $< -c

$(TARGET):		$(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) 

clean:
	-rm -f *.o $(TARGET) core core.*
