#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment,
#** also known as GENESIS 3 base code.
#**           copyright (C) 2003- 2006 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/
# $Id: $
#
# $Log: $
#

# Use the options below for compiling on GCC3. Pick your favourite
# optimization settings.
#CFLAGS  =	-g -Wall -pedantic -DDO_UNIT_TESTS -DNO_OFFSETOF
#CFLAGS  =	-O3 -Wall -pedantic -DNO_OFFSETOF
#CFLAGS  =	-O3 -pg -Wall -pedantic -DNO_OFFSETOFF

# Use the options below for compiling on GCC4.0
# ANSI C++ and hence gcc4 have some strange error messages that emanate
# from offsetof, even when it does the right thing. The 
# -Wno-invalid-offsetof flag suppresses these silly warnings.
#  For Debian/Ubuntu 6.06, we need to add a few more compiler flags to
#  help it through the genesis parser, which is littered with ifdefs.
CFLAGS  =	-g -Wall -pedantic -DDO_UNIT_TESTS -DYYMALLOC -DYYFREE -Wno-invalid-offsetof
#CFLAGS  =	-g -Wall -pedantic -DDO_UNIT_TESTS -Wno-invalid-offsetof
#CFLAGS  =	-O3 -Wall -pedantic -Wno-invalid-offsetof

# Use the options below for compiling on GCC4.1
# GNU C++ 4.1 and newer will need -ffriend-injection
#
#CFLAGS  =	-g -Wall -pedantic -DDO_UNIT_TESTS -DYYMALLOC -DYYFREE -Wno-invalid-offsetof -ffriend-injection
#CFLAGS  =	-g -Wall -pedantic -DDO_UNIT_TESTS -Wno-invalid-offsetof -ffriend-injection


# Libraries are defined below. For now we do not use threads.
SUBLIBS = 
#LIBS = 		-lm -lpthread
LIBS = 		-lm

# Here we decide if we want to use MPI and the parallel library
# Uncomment the line below if you do.
# The -DMPICH_IGNORE_CXX_SEEK flag is because of a bug in the
# MPI-2 standard. Hopefully it won't affect you, but if it does use
# the version of PARALLEL_FLAGS that defines this flag.
#PARALLEL_FLAGS = -DUSE_MPI
#PARALLEL_FLAGS = -DUSE_MPI -DMPICH_IGNORE_CXX_SEEK
#PARALLEL_DIR = parallel
#PARALLEL_LIB = parallel/parallel.o

# Depending on whether we compile with MPI, you may need to change the
# CXX compiler below
#
#CXX = mpicxx

#
# If you do use mpicxx, comment out the version below.
#
CXX = g++

LD = ld

#
# moved genesis_parser to beginning since it generates code
#
SUBDIR = genesis_parser basecode maindir randnum builtins scheduling kinetics biophysics textio hsolve $(PARALLEL_DIR) utility zbiophysics hsolve1/moosesolve

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
	hsolve/hsolve.o \
	utility/utility.o \
	zbiophysics/zbiophysics.o \
	hsolve1/moosesolve/hsolve1.o

moose: libs $(OBJLIBS) $(PARALLEL_LIB)
	$(CXX) $(CFLAGS) $(PARALLEL_FLAGS) $(OBJLIBS) $(PARALLEL_LIB) $(LIBS) -o moose
	@echo "Moose compilation finished"

libmoose.so: libs
	$(CXX) -G $(LIBS) -o libmoose.so
	@echo "Created dynamic library"

libs:
	@(for i in $(SUBDIR); do echo cd $$i; cd $$i; make CXX="$(CXX)" CFLAGS="$(CFLAGS) $(PARALLEL_FLAGS)" LD="$(LD)" LIBS="$(SUBLIBS)"; cd ..; done)
	@echo "All Libs compiled"

mpp: preprocessor/*.cpp preprocessor/*.h
	@( rm -f mpp; cd preprocessor; make CXX="$(CXX)" CFLAGS="$(CFLAGS)"; ln mpp ..; cd ..)

default: moose mpp

clean:
	@(for i in $(SUBDIR) ; do echo cd $$i; cd $$i; make clean; cd ..; done)
	-rm -rf moose mpp core.* 
