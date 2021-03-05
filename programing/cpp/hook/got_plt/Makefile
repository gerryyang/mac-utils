
CXXFLAGS = -Werror -Wall -g -pipe -O2 -Wno-unused-variable -Wno-unused-function
CFLAGS = -Werror -Wall -g -pipe -O2 -Wno-unused-variable -Wno-unused-function

INCLUDE = -I. -I./plthook
LIBPATH = -L.
LIBS = -ltesta -ltesta_hook -lplthook -ldl 
TARGET3_LIBS = -ltesta -ltesta_hook -ldl 
TARGET5_LIBS = -ltestb -ltestb_hook -lplthook -ldl 

OPTIONS = -Wl,-rpath,./

TARGET1 = demo1
TARGET1_OBJS = demo1.o

TARGET2 = demo2
TARGET2_OBJS = demo2.o

TARGET3 = demo3
TARGET3_OBJS = demo3.o

TARGET4 = demo4
TARGET4_OBJS = demo4.o

TARGET5 = demo5
TARGET5_OBJS = demo5.o

.PHONY: clean plthook

TARGET_LIST = $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5)

all: testa testa_hook testb testb_hook plthook $(TARGET_LIST)

testa:
	$(CXX) $(CFLAGS) testa.c -fPIC -shared -o libtesta.so

testa_hook:
	$(CXX) $(CFLAGS) testa_hook.c -fPIC -shared -o libtesta_hook.so

testb:
	$(CXX) $(CFLAGS) testb.cc -fPIC -shared -o libtestb.so

testb_hook:
	$(CXX) $(CFLAGS) testb_hook.cc -fPIC -shared -o libtestb_hook.so

plthook:
	$(CXX) $(CFLAGS) plthook/plthook_elf.c -fPIC -shared -o libplthook.so

$(TARGET1): $(TARGET1_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS) $(OPTIONS)
	@echo "build $(TARGET1) ok"

$(TARGET2): $(TARGET2_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS) $(OPTIONS)
	@echo "build $(TARGET2) ok"

$(TARGET3): $(TARGET3_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(TARGET3_LIBS) $(OPTIONS)
	@echo "build $(TARGET3) ok"

$(TARGET4): $(TARGET4_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS) $(OPTIONS)
	@echo "build $(TARGET4) ok"

$(TARGET5): $(TARGET5_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(TARGET5_LIBS) $(OPTIONS)
	@echo "build $(TARGET5) ok"

install:
	@echo "nothing to install"

clean:
	rm -f *.o *.so $(TARGET_LIST)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<



