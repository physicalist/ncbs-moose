CHARM_PATH = #
CHARM_LIBS = $(CHARM_PATH)/lib
INCFLAGS = -I$(CHARM_PATH)/include -I../basecode -I../shell -I../randnum -I../pymoose -I../charm 

APP_FLAGS = # 
OPTS = -O0 -g $(APP_FLAGS)
CPPFLAGS += $(INCFLAGS) 
CXXFLAGS += $(OPTS) $(CPPFLAGS)
LDFLAGS += $(OPTS) -lccs-client -language charm++ 

CHARMC = $(CHARM_PATH)/bin/charmc

CXX = $(CHARMC)
CC = $(CXX)
AR = ar q 
CXX_DEPEND = $(CXX) -M -MM -MG -Wall $(CXXFLAGS) 
CFLAGS = $(OPTS) $(DEFINE_FLAGS) -g 

OBJECTS = Main.o \
          rtHHNetwork.o \
          speedTestMultiNodeIntFireNetwork.o \
          LaunchJob.o \
          ParamStorage.o 

LINK_OBJECTS = ../shell/ShellProxy.o \
               ../basecode/SetGetCcsClient.o \
               ../basecode/DefSetGetCcsHandlerStrings.o \
               ../basecode/doubleEq.o \
               ../basecode/CcsId.o \
               ../basecode/CcsObjId.o \
               ../basecode/CcsId_parser.o \
               ../basecode/CcsObjId_parser.o \
               ../basecode/CcsCinfo.o \
               ../basecode/CcsFinfo.o \
               ../charm/MooseEnv.o \
               ../msg/Msg_parser.o \
               ../randnum/_randnum.o

SRC = Main.cpp \
      rtHHNetwork.cpp \
      speedTestMultiNodeIntFireNetwork.cpp \
      LaunchJob.cpp \
      ParamStorage.cpp 

TARGET = test
all: $(TARGET) 

$(TARGET): $(OBJECTS) Makefile.dep  
	$(CHARMC) -o $(TARGET) $(LDFLAGS) $(OBJECTS) $(LINK_OBJECTS)

%.decl.h %.def.h : %.ci
	$(CHARMC) $(APP_FLAGS) -E $(CPPFLAGS) $<

%.o: Makefile

clean:
	rm -f core* *.a *.o $(OBJECTS) *~ $(TARGET) *.decl.h *.def.h charmrun conv-host 

depends:
	$(CXX_DEPEND) $(SRC) | while read i;do echo $$i| awk -F' ' '{for (i=1;i<NF;++i) print $$i" \\"}';echo;done|grep -v "$(CHARM_PATH)/bin" > Makefile.dep 

.PHONY: all docs depends 

include Makefile.dep
