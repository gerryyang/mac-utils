
CXXFLAGS = -Wall -g -pipe -O2
CFLAGS = -Wall -g -pipe -O2 -Wno-unused-variable

INCLUDE = -I.
LIBPATH = -L.
LIBS = 

TARGET = test
TARGET_OBJS = test.o

.PHONY: clean

all: $(TARGET)

$(TARGET): $(TARGET_OBJS)
	$(CC) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS)
	@echo "build $(TARGET) ok"

install:
	@echo "nothing to install"

clean:
	rm -f $(TARGET) *.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

