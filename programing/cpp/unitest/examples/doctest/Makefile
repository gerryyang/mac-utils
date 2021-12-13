.PHONY: all clean test

CXXFLAGS = -std=c++11 -Wall -g -pipe -O2
CFLAGS = -Wall -g -pipe -O2 -Wno-unused-variable

INCLUDE ?= -I../../doctest/doctest/

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
