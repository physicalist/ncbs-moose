CHARM_PATH = #
DEFINE_FLAGS = -DUSE_CHARMPP

INCPATH = -I$(CHARM_PATH)/tmp/libs/ck-libs/NDMeshStreamer -I ../shell -I../basecode -I../scheduling -I../msg 
OPTS = -g -O0 
CXXFLAGS += $(OPTS) $(DEFINE_FLAGS)
LDFLAGS += $(OPTS) -L. -L$(CHARM_PATH)/lib -language charm++ #-tracemode projections 

CHARMC = $(CHARM_PATH)/bin/charmc $(INCPATH)

CXX = $(CHARMC) 
CC = $(CXX)
AR = ar q 
CXX_DEPEND = $(CXX) -M -MM -MG $(CXXFLAGS)

OBJECTS = ElementContainer.o Main.o LookupHelper.o ParallelTestHelper.o SocketReply.o MooseEnv.o pup_util.o 
SRCS = ElementContainer.cpp Main.cpp LookupHelper.cpp ParallelTestHelper.cpp SocketReply.cpp MooseEnv.cpp $(CHARM_PATH)/tmp/pup_util.C

TARGET = _moose_charm.o
all: $(TARGET) 

$(TARGET): $(OBJECTS) 
	$(LD) -r -o $(TARGET) $(OBJECTS) 

%.decl.h %.def.h : %.ci
	$(CHARMC) -E $(OPTS) $(DEFINE_FLAGS) $<

pup_util.o: $(CHARM_PATH)/tmp/pup_util.C
	$(CHARMC) -c -o $@ $(CXXFLAGS) -DSTANDALONE_PUP $(INCPATH) $(CHARM_PATH)/tmp/pup_util.C

%.o: Makefile

clean:
	rm -f core* $(OBJECTS) *~ $(TARGET) *.decl.h *.def.h charmrun conv-host 

projclean:
	rm -f *.gz *.projrc *.sts *.topo

depends:
	$(CXX_DEPEND) $(SRCS) | while read i;do echo $$i| awk -F' ' '{for (i=1;i<NF;++i) print $$i" \\"}';echo;done|grep -v "$(CHARM_PATH)/bin" > Makefile.dep

.PHONY: all docs dist clean depend test

include Makefile.dep
