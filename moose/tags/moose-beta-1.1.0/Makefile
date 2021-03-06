#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment,
#** also known as GENESIS 3 base code.
#**           copyright (C) 2003- 2006 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU General Public License version 2
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/
#######################################################################
# NOTE:
# This Makefile is compatible with _GNU Make_.
# This does not work with nmake or borland make.
# You may have to specify some variables when calling gnu make as 
# described in the comments below. The defaults should work on most
# Unix clones. 
########################################################################

# Linux compilation:
# We recommend two levels of compilation: either full debug, with gdb,
# unit tests and all the rest, or an optimized version for production
# simulations, without any unit tests or assertions. If you want some
# intermediate levels, edit the flags.
######################################################################
#
#     ADDITIONAL COMMANDLINE VARIABLES FOR MAKE
#
######################################################################     
# The variable BUILD determines if it should be optimized (release)
# or a debug version (default).
# make can be run with a command line parameter like below:
# make clean BUILD=debug
# make BUILD=debug
# another option is to define BUILD as an environment variable:
# export BUILD=debug
# make clean
# make
#
# There are some more variables which just need to be defined for 
# controlling the compilation and the value does not matter. These are:
#
# USE_GSL - use GNU Scientific Library for integration in kinetic simulations
# 
# USE_READLINE - use the readline library which provides command history and 
# 		better command line editing capabilities
# 
# GENERATE_WRAPPERS - useful for python interface developers. The binary created 
# 		with this option looks for a directory named 'generated' in the
# 		working directory and creates a wrapper class ( one .h file 
# 		and a .cpp file ) and partial code for the swig interface file
# 		(pymoose.i). These files with some modification can be used for
# 		generating the python interface using swig.
#
# USE_MPI - compile with support for parallel computing through MPICH library
#

# BUILD (= debug, release)
ifndef BUILD
BUILD=debug
endif

# PLATFORM (= Linux, win32, Darwin)
#If using mac uncomment the following lines
# PLATFORM=mac
#export PLATFORM

# Get the processor architecture - i686 or x86_64
# All these should be taken care of in a script, not in the 
# Makefile. But we are
ifndef MACHINE
MACHINE=i686 
endif
# We are assuming all non-win32 systems to be POSIX compliant
# and thus have the command uname for getting Unix system name
ifneq ($(OSTYPE),win32)
MACHINE=$(shell uname -m)
endif

# Debug mode:
ifeq ($(BUILD),debug)
CFLAGS = -g -Wall -Wno-long-long -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER
endif
# Optimized mode:
ifeq ($(BUILD),release)
CFLAGS  = -O3 -Wall -Wno-long-long -pedantic -DNDEBUG -DUSE_GENESIS_PARSER  
endif
##########################################################################
#
# MAC OS X compilation, Debug mode:
ifeq ($(OSTYPE),Darwin)
CFLAGS += -Wno-deprecated -force_cpusubtype_ALL -mmacosx-version-min=10.4
pymoose: CFLAGS += -bundle -fno-common /usr/bin/python -flat_namespace -undefined suppress -I/Library/Frameworks/Python.framework/Versions/2.5/include/python2.5
pymoose: LDFLAGS+= -bundle -fno-common -L/Library/Frameworks/Python.framework/Versions/2.5/lib/python2.5/config/
endif
# Use the options below for compiling on GCC4.1
# GNU C++ 4.1 and newer might need -ffriend-injection
#
#CFLAGS  =	-g -Wall -pedantic -DDO_UNIT_TESTS -ffriend-injection -DUSE_GENESIS_PARSER

##########################################################################
#
# Don't mess with stuff below!
#
##########################################################################


# Libraries are defined below. For now we do not use threads.
SUBLIBS = 
#LIBS =	-lm -lpthread
LIBS = 	-lm 
##########################################################################
#
# Developer options (Don't try these unless you are writing new code!)
#
# For generating python interface:
# Do remember that you have to create a directory named "generated" 
# in the working directory of moose. Also you have to do some editing 
# to get the generated code to work. 
# Although this binary of MOOSE is verbose in its complaints, is completely harmless 
# except for the overhead of  checks for the existence of a few files at startup.
ifdef GENERATE_WRAPPERS
CFLAGS += -DGENERATE_WRAPPERS
endif

# For parallel (MPI) version:
ifdef USE_MPI
CFLAGS += -DUSE_MPI
endif




# The -DMPICH_IGNORE_CXX_SEEK flag is because of a bug in the
# MPI-2 standard. Hopefully it won't affect you, but if it does use
# the version of CFLAGS that defines this flag:
#CFLAGS = -g -Wall -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER -DUSE_MPI -DMPICH_IGNORE_CXX_SEEK


#use this for readline library
#CFLAGS = -g -Wall -pedantic -DDO_UNIT_TESTS -DUSE_GENESIS_PARSER -DUSE_READLINE


# To use GSL, pass USE_GSL=true ( anything on the right will do) in make command line
ifdef USE_GSL
LIBS+= -lgsl -lgslcblas
CFLAGS+= -DUSE_GSL
endif

# To compile with readline support pass USE_READLINE=true in make command line
ifdef USE_READLINE
LIBS+= -lreadline
CFLAGS+= -DUSE_READLINE
endif


# For 64 bit Linux systems add paths to 64 bit libraries 
ifeq ($(OSTYPE),Linux)
ifeq ($(MACHINE),x86_64)
LIBS=-L/lib64 -L/usr/lib64 $(LIBS) 
endif
endif

# Here we automatically change compilers to deal with MPI.
ifneq (,$(findstring DUSE_MPI,$(CFLAGS)))
       CXX = mpicxx
       PARALLEL_DIR = parallel
       PARALLEL_LIB = parallel/parallel.o
else
       CXX = g++
endif

LD = ld

SUBDIR = genesis_parser basecode connections shell element maindir scheduling biophysics hsolve kinetics builtins $(PARALLEL_DIR) utility randnum 


OBJLIBS =	\
	basecode/basecode.o \
	connections/connections.o \
	maindir/maindir.o \
	genesis_parser/SLI.o \
	element/element.o \
	shell/shell.o \
	utility/utility.o \
	randnum/randnum.o	\
	scheduling/scheduling.o \
	biophysics/biophysics.o \
	hsolve/hsolve.o \
	kinetics/kinetics.o \
	builtins/builtins.o \
# robots/robots.o \

# example/example.o 

export CFLAGS
export LD
export LIBS

moose: libs $(OBJLIBS) $(PARALLEL_LIB)
	$(CXX) $(CFLAGS) $(OBJLIBS) $(PARALLEL_LIB) $(LIBS) -o moose
	@echo "Moose compilation finished"

libmoose.so: libs
	$(CXX) -G $(LIBS) -o libmoose.so
	@echo "Created dynamic library"

pymoose: CFLAGS += -fPIC 
pymoose: SUBDIR += pymoose
pymoose: libs $(OBJLIBS) 
	$(MAKE) -C $@

libs:
	@(for i in $(SUBDIR); do $(MAKE) -C $$i; done)
	@echo "All Libs compiled"

mpp: preprocessor/*.cpp preprocessor/*.h
	@( rm -f mpp; cd preprocessor; make CXX="$(CXX)" CFLAGS="$(CFLAGS)"; ln mpp ..; cd ..)

default: moose mpp
clean: SUBDIR += pymoose
clean:
	@(for i in $(SUBDIR) ; do $(MAKE) -C $$i clean;  done)
	-rm -rf moose mpp core.* DOCS/html *.so *.py *.pyc
