#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2007 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = _shell.o

OBJ = \
	Shell.o	\
	ShellCopy.o	\
	ShellSetGet.o	\
	ShellThreads.o	\
	ProcessLoop.o	\
	LoadModels.o \
	Neutral.o	\
	Wildcard.o	\
	testShell.o	\

ifeq ($(BUILD),charm++)
OBJ += ShellCcsInterface.o \
       ShellCcsInterfaceOperation.o \
       ShellProxy.o 
endif

HEADERS = \
	../basecode/header.h \
	../basecode/FuncBarrier.h \
	../basecode/ReduceBase.h \
	../basecode/ReduceMax.h \

ifeq ($(BUILD),charm++)
HEADERS += ../charm/moose.decl.h
endif


default: $(TARGET)

$(OBJ)	: $(HEADERS)
ifneq ($(BUILD),charm++)
Shell.o:	Shell.h Neutral.h ../scheduling/Clock.h ../sbml/SbmlWriter.h
else
Shell.o:	../charm/moose.decl.h Shell.h Neutral.h ../scheduling/Clock.h ../sbml/SbmlWriter.h
endif
ShellCopy.o:	Shell.h Neutral.h
ShellSetGet.o:	Shell.h
ShellThreads.o:	Shell.h Neutral.h ../scheduling/Clock.h
ProcessLoop.o:	Shell.h ../basecode/FuncBarrier.h
LoadModels.o:	Shell.h Neutral.h ../kinetics/ReadKkit.h ../biophysics/ReadCell.h
Neutral.o:	Neutral.h ../basecode/ElementValueFinfo.h
Wildcard.o:	Wildcard.h Shell.h Neutral.h ../basecode/ElementValueFinfo.h
testShell.o:	Wildcard.h Shell.h Neutral.h ../scheduling/Tick.h ../scheduling/testScheduling.h ../builtins/Arith.h ../basecode/SparseMatrix.h ../msg/SparseMsg.h ../msg/SingleMsg.h ../basecode/SetGet.h

ifeq ($(BUILD),charm++)
ShellCcsInterface.o : ../charm/moose.decl.h ShellCcsInterfaceOperation.h ShellCcsInterface.h 
ShellCcsInterfaceOperation.o : ShellCcsInterfaceOperation.h ShellProxyHelpers.h 
ShellProxy.o : ShellProxy.h ../charm/moose.decl.h ../basecode/CcsPackUnpack.h ShellProxyHelpers.h
endif

.cpp.o:
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I. -I../basecode -I../msg $< -c

$(TARGET):		$(OBJ) $(SMOLDYN_OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) 

clean:
	-rm -f *.o $(TARGET) core core.* *.decl.h *.def.h
