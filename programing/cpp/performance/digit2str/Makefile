CXXFLAGS = -Wall -O2 -g -pipe -std=c++2a
CFLAGS = -Wall -O2 -g -pipe 

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
