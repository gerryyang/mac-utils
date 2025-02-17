.PHONY: all clean test

CXXFLAGS = -std=c++11 -Wall -g -pipe -O2
CFLAGS = -Wall -g -pipe -O2 -Wno-unused-variable

# use v2.4.11 dectest.h to test
# refer https://github.com/doctest/doctest/releases/tag/v2.4.11
INCLUDE ?= -I.

TARGET = demo
TARGET_OBJS = demo.o

all: $(TARGET)

$(TARGET): $(TARGET_OBJS)
	$(CXX) -o $@ $< 
	@echo "build $(TARGET) ok"

clean:
	@rm -f *.o $(TARGET)

test: $(TARGET)
	./$<

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<
