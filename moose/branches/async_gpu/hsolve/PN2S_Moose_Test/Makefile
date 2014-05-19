CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -lm -I/usr/include/python2.7

OBJS =		PN2S_Moose_Test.o

LIBS =

TARGET =	PN2S_Moose_Test

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) -lm -lpython2.7 -L/usr/lib/python2.7/config

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) *.plot
