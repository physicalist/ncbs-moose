#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment,
#** also known as GENESIS 3 base code.
#**           copyright (C) 2004 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = _sbml.o

OBJ = \
	SbmlWriter.o \
	#~ SbmlReader.o \

HEADERS = \
	../basecode/header.h

default: $(TARGET)

$(OBJ)	: $(HEADERS)
SbmlWriter.o:	SbmlWriter.h
#~ SbmlReader.o:	SbmlReader.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -I. -I../basecode -I../msg $< -c

$(TARGET): $(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ)
clean:
	rm -f *.o $(TARGET) core core.*
