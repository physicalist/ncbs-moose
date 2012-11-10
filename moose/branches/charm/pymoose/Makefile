# Makefile
# 
# Author: Subhasis Ray
# Created: 2011-03-10 12:10:32 (+0530)
TARGET = _pymoose.o
# CXXFLAGS += -I../basecode -I../msg -I../shell
.PHONY: clean

HEADERS = moosemodule.h

OBJ = moosemodule.o

PYMOOSE_DEPS = moosemodule.cpp $(OBJ) $(HEADERS) ../basecode/main.cpp ../utility/_utility.o ../basecode/_basecode_pymoose.o

ifeq ($(BUILD), charm++)
PYMOOSE_DEPS += MooseParams.h ../shell/ShellProxy.h
endif

$(TARGET): $(PYMOOSE_DEPS) 
	$(LD) -r -o $(TARGET) $(OBJ) $(LDFLAGS)
.cpp.o:
	$(CXX) $(CXXFLAGS) $< -c

clean: 
	-rm -f *.o

