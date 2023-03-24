
PB_PATH = $(HOME)/tools/protobuf-3.15.8-install
BOOST_PATH = $(HOME)/tools/boost_1_76_0
CELERO_PATH = $(HOME)/tools/Celero

CXXFLAGS = -Wall -g -pipe -std=c++11 -Wno-unused-result
CFLAGS = -Wall -g -pipe -O2

INCLUDE = -I. -I$(PB_PATH)/include -I$(BOOST_PATH) -I$(CELERO_PATH)/include
LIBPATH = -L. -L$(PB_PATH)/lib -lprotobuf -L$(CELERO_PATH) -lcelero
LIBS =

TARGET = press
TARGET_OBJS = press.o addressbook.pb.o

TARGET2 = encode_decode
TARGET_OBJS2 = encode_decode.o addressbook.pb.o

CELERO_BANCHMARK = celero_benchmark
CELERO_BANCHMARK_OBJS = celero_benchmark.o addressbook.pb.o

.PHONY: clean

all: $(TARGET) $(CELERO_BANCHMARK)

$(TARGET): $(TARGET_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS) -Wl,-rpath $(PB_PATH)/lib
	@echo "build $(TARGET) ok"

$(TARGET2): $(TARGET_OBJS2)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS) -Wl,-rpath $(PB_PATH)/lib
	@echo "build $(TARGET2) ok"

$(CELERO_BANCHMARK) : $(CELERO_BANCHMARK_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS) -Wl,-rpath $(CELERO_PATH) -Wl,-rpath $(PB_PATH)/lib
	@echo "build $(CELERO_BANCHMARK) ok"

install:
	@echo "nothing to install"

clean:
	rm -f $(TARGET) $(TARGET2) $(CELERO_BANCHMARK) *.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<


