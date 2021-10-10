CXXFLAGS = -Wall -g -pipe -std=c++2a -O2
CFLAGS = -Wall -g -pipe -O2

CXX=g++
CC=gcc

INCLUDE = -I./deps/fmt-8.0.1/include
LIBPATH = -L.
LIBS = 

TARGET = test
TARGET_OBJS = test.o

.PHONY: clean

all: $(TARGET)

$(TARGET): $(TARGET_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ 
	@echo "build $(TARGET) ok"

install:
	@echo "nothing to install"

clean:
	rm -f $(TARGET) *.o

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<
