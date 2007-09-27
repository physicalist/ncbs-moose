CFLAGS += -I..
# the two lines below are for easy inclusion of libxml++
CFLAGS += $(shell pkg-config libxml++-2.6 --cflags) 

TARGET = utility.o

OBJ = \
	Configuration.o \
	StringUtil.o


default: $(TARGET)

.cpp.o:
	$(CXX) $(CFLAGS) $(shell pkg-config libxml++-2.6 --cflags) $< -c

$(TARGET): $(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) 

clean:
	-rm -f *.o $(TARGET) core core.*
