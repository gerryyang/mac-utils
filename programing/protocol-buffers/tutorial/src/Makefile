
PB_PATH = /home/ubuntu/tools/protobuf-3.15.8-install

CXXFLAGS = -Wall -g -pipe -O2 -std=c++11 
CFLAGS = -Wall -g -pipe -O2

INCLUDE = -I. -I$(PB_PATH)/include
LIBPATH = -L. -L$(PB_PATH)/lib -lprotobuf
LIBS =

TARGET = press
TARGET_OBJS = press.o addressbook.pb.o

.PHONY: clean

all: $(TARGET)

$(TARGET): $(TARGET_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS) -Wl,-rpath $(PB_PATH)/lib
	@echo "build $(TARGET) ok"

install:
	@echo "nothing to install"

clean:
	rm -f $(TARGET) *.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<


