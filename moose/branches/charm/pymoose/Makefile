# Makefile
# 
# Author: Subhasis Ray
# Created: 2011-03-10 12:10:32 (+0530)
TARGET = _pymoose.o
# CXXFLAGS += -I../basecode -I../msg -I../shell
.PHONY: clean

HEADERS = moosemodule.h 

OBJ = moosemodule.o 

PYMOOSE_DEPS = moosemodule.cpp $(OBJ) $(HEADERS) ../basecode/main.cpp ../utility/_utility.o 

ifneq ($(BUILD), charm++)
PYMOOSE_DEPS += ../basecode/_basecode_pymoose.o ../charm/MooseEnv.o
else
PYMOOSE_DEPS += MooseParams.h ../shell/ShellProxy.h ../charm/MooseEnv.h
endif

$(TARGET): $(PYMOOSE_DEPS) ../basecode/CcsPackUnpack.h 
	$(LD) -r -o $(TARGET) $(OBJ) $(LDFLAGS)
.cpp.o:
	$(CXX) $(CXXFLAGS) $< -c

clean: 
	-rm -f *.o

