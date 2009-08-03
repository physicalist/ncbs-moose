#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2007 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = glcell.o

OBJ = \
	GLcell.o	\

HEADERS = \
	../basecode/header.h \
	../connections/Conn.h \
	../basecode/Element.h \

OSG_INCLUDE = \
	/usr/local/include/osg/include \

default: $(TARGET)

$(OBJ)	: $(HEADERS)
GLcell.o:	GLcell.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(GSL_FLAGS) -I../ -I../basecode -I$(OSG_INCLUDE) $< -c

$(TARGET):		$(OBJ) $(GSL_OBJ) $(HEADERS) 
	$(LD) -r -o $(TARGET) $(OBJ) $(GSL_OBJ)  $(GSL_LIBS)

clean:
	-rm -f *.o $(TARGET) core core.*
