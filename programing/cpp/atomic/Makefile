
CXXFLAGS = -Wall -g -pipe -O2 -Wno-unused-but-set-variable
CFLAGS = -Wall -g -pipe -O2 -Wno-unused-but-set-variable

INCLUDE = -I.
LIBPATH = -L.
LIBS = 

TARGET = atomic_test
TARGET_OBJS = atomic_counter.o atomic_test.o

.PHONY: clean

all: $(TARGET)

$(TARGET): $(TARGET_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS)
	@echo "build $(TARGET) ok"

install:
	@echo "nothing to install"

clean:
	rm -f $(TARGET) *.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

