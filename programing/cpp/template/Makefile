.PHONY: all clean test

#CXX = /usr/bin/clang++

#CXXFLAGS = -std=c++11 -Wall -g -pipe -O2 -DNDEBUG
CXXFLAGS = -std=c++11 -Wall -g -pipe -O0
CFLAGS = -Wall -g -pipe -O2 -Wno-unused-variable

INCLUDE ?= -I./

TARGET = demo
TARGET_OBJS = util.o demo.o

all: $(TARGET)

$(TARGET): $(TARGET_OBJS)
	$(CXX) -o $@ $^
	@echo "build $(TARGET) ok"

clean:
	@rm -f *.o $(TARGET)

test: $(TARGET)
	./$<

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<
