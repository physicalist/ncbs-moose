CHARM_PATH = $(HOME)/work/charm-soc/charm/net-linux
DEFINE_FLAGS = -DUSE_CHARMPP

INCPATH = -I$(CHARM_PATH)/tmp/libs/ck-libs/NDMeshStreamer -I ../shell -I../basecode -I../scheduling -I../msg 
OPTS = -g -O3 
CXXFLAGS += $(OPTS) $(DEFINE_FLAGS)
LDFLAGS += $(OPTS) -L. -L$(CHARM_PATH)/lib -language charm++ -module completion -module NDMeshStreamer #-tracemode projections 

CHARMC = $(CHARM_PATH)/bin/charmc $(INCPATH)

CXX = $(CHARMC) 
CC = $(CXX)
AR = ar q 
CXX_DEPEND = $(CXX) -M -MM -MG $(CXXFLAGS)

OBJECTS = ElementContainer.o Main.o LookupHelper.o
SRCS = ElementContainer.cpp Main.cpp LookupHelper.cpp

TARGET = _moose_charm.o
all: $(TARGET) 

$(TARGET): $(OBJECTS) 
	$(LD) -r -o $(TARGET) $(OBJECTS) 

%.decl.h %.def.h : %.ci
	$(CHARMC) -E $(OPTS) $(DEFINE_FLAGS) $<

%.o: Makefile

clean:
	rm -f core* $(OBJECTS) *~ $(TARGET) *.decl.h *.def.h charmrun conv-host 

projclean:
	rm -f *.gz *.projrc *.sts *.topo

depends:
	$(CXX_DEPEND) $(SRCS) | while read i;do echo $$i| awk -F' ' '{for (i=1;i<NF;++i) print $$i" \\"}';echo;done|grep -v "$(CHARM_PATH)/bin" > Makefile.dep

.PHONY: all docs dist clean depend test

include Makefile.dep
