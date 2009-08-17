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

TARGETCLIENT = glcellclient

HEADERS = \
	../basecode/header.h \
	../connections/Conn.h \
	../basecode/Element.h \

OSG_INCLUDE = \
	/usr/local/include/osg/include \

OSG_LIBS = \
	-L/usr/local/lib -losg -losgDB -losgGA -losgViewer -lOpenThreads \

BOOST_INCLUDE = \
	/usr/local/include/boost-1_39 \

BOOST_LIBS = \
	-L/lib -lboost_serialization-xgcc40-mt \

default: $(TARGET) $(TARGETCLIENT) 

$(OBJ)	: $(HEADERS)
GLcell.o:	GLcell.h
GLcellCompartment.o:	GLcellCompartment.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -I../ -I../basecode -I$(OSG_INCLUDE) -I$(BOOST_INCLUDE) GLcell.cpp -c

$(TARGETCLIENT):
	$(CXX) $(CXXFLAGS) -I$(BOOST_INCLUDE) -o glcellclient GLcellClient.cpp $(BOOST_LIBS) $(OSG_LIBS)

$(TARGET): $(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ)

clean:
	rm -f *.o $(TARGET) $(TARGETCLIENT) core core.*
