# Makefile
# 
# Author: Subhasis Ray
# Created: 2011-03-10 12:10:32 (+0530)
TARGET = pymoose.o

.PHONY: clean

OBJ = moosemodule.o

$(TARGET): $(OBJ)
	$(LD) -r -o $(TARGET) $(OBJ) 

clean: 
	-rm -f *.o

