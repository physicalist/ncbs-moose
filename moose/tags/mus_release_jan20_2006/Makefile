#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment,
#** also known as GENESIS 3 base code.
#**           copyright (C) 2003- 2005 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/
# $Id: $
#
# $Log: $
#

#CFLAGS  =	-O
#CFLAGS  =	-g -fhandle-exceptions 
#CFLAGS  =	-g -D_EXCEPTIONS
#CFLAGS  =	-g -Wall -pedantic -DDO_UNIT_TESTS -DNO_OFFSETOF
#CFLAGS  =	-O -pg -Wall
CFLAGS  =	-O3 -Wall -pedantic
#LIBS = 		-lm -liostream
SUBLIBS = 
#LIBS = 		-lm -lpthread
LIBS = 		-lm
CXX = g++
LD = ld

SUBDIR = basecode maindir randnum builtins genesis_parser scheduling kinetics biophysics textio

OBJLIBS =	\
	basecode/basecode.o \
	scheduling/scheduling.o \
	randnum/randnum.o \
	maindir/maindir.o \
	genesis_parser/SLI.o \
	builtins/builtins.o \
	textio/textio.o \
	kinetics/kinetics.o \
	biophysics/biophysics.o \


libs:
	@(for i in $(SUBDIR); do echo cd $$i; cd $$i; make CXX="$(CXX)" CFLAGS="$(CFLAGS)" LD="$(LD)" LIBS="$(SUBLIBS)"; cd ..; done)
	@echo "All Libs compiled"

moose: libs $(OBJLIBS)
	$(CXX) $(CFLAGS) $(OBJLIBS) $(LIBS) -o moose
	@echo "Moose compilation finished"

mpp: preprocessor/*.cpp preprocessor/*.h
	@( rm -f mpp; cd preprocessor; make CXX="$(CXX)" CFLAGS="$(CFLAGS)"; ln mpp ..; cd ..)

default: moose mpp

clean:
	@(for i in $(SUBDIR) ; do echo cd $$i; cd $$i; make clean; cd ..; done)
	-rm -rf moose mpp core.* 
