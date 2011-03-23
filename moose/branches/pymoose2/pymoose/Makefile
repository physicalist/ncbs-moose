# Makefile
# 
# Author: Subhasis Ray
# Created: 2011-03-10 12:10:32 (+0530)
TARGET = pymoose.o
CXXFLAGS += -I../basecode -I../msg -I../shell
.PHONY: clean

HEADERS = pymoose.h \
	pymoose_Neutral.h \
	moosemodule.h \


OBJ = moosemodule.o \
	pymoose_Neutral.o \


$(TARGET): $(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ)

clean: 
	-rm -f *.o

