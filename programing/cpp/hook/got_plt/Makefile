
CXXFLAGS = -std=c++11 -Werror -Wall -g -pipe -O2 -pg -Wno-unused-variable -Wno-unused-function
CFLAGS = -Werror -Wall -g -pipe -O2 -Wno-unused-variable -Wno-unused-function

INCLUDE = -I. -I./plthook
LIBPATH = -L.
LIBS = -ltesta -ltesta_hook -lplthook -ldl
TARGET3_LIBS = -ltesta -ltesta_hook -ldl
TARGET5_LIBS = -ltestb -ltestb_hook -lplthook -ldl
TARGET6_LIBS = -ltestc -ltestc_hook -lplthook -ldl
TARGET7_LIBS = -ltestc -ltestc_hook -lplthook -ldl
TARGET8_LIBS = -ltestd -ltestd_hook -lplthook -ldl

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

# hook cpp so function
TARGET6 = demo6
TARGET6_OBJS = demo6.o

# hook cpp so function
TARGET7 = demo7
TARGET7_OBJS = demo7.o

# hook cpp so function
TARGET8 = demo8
TARGET8_OBJS = demo8.o

.PHONY: clean plthook

TARGET_LIST = $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6) $(TARGET7) $(TARGET8)

all: testa testa_hook testb testb_hook testc testc_hook testd testd_hook plthook plthook_static $(TARGET_LIST)

testa:
	$(CC) $(CFLAGS) testa.c -fPIC -shared -o libtesta.so

testa_hook:
	$(CC) $(CFLAGS) testa_hook.c -fPIC -shared -o libtesta_hook.so

testb:
	$(CXX) $(CXXFLAGS) testb.cc -fPIC -shared -o libtestb.so

testb_hook:
	$(CXX) $(CXXFLAGS) testb_hook.cc -fPIC -shared -o libtestb_hook.so

testc:
	$(CXX) $(CXXFLAGS) testc.cc -fPIC -shared -o libtestc.so

testc_hook:
	$(CXX) $(CXXFLAGS) $(INCLUDE) testc_hook.cc -fPIC -shared -o libtestc_hook.so

testd:
	$(CXX) $(CXXFLAGS) testd.cc -fPIC -shared -o libtestd.so

testd_hook:
	$(CXX) $(CXXFLAGS) $(INCLUDE) testd_hook.cc -fPIC -shared -o libtestd_hook.so

plthook:
	$(CC) $(CFLAGS) plthook/plthook_elf.c -fPIC -shared -o libplthook.so

plthook_static:
	$(CC) $(CFLAGS) -c plthook/plthook_elf.c -fPIC -o plthook_elf.o
	ar rcs libplthook.a plthook_elf.o

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

$(TARGET6): $(TARGET6_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(TARGET6_LIBS) $(OPTIONS)
	@echo "build $(TARGET6) ok"

$(TARGET7): $(TARGET7_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(TARGET7_LIBS) $(OPTIONS)
	@echo "build $(TARGET7) ok"

$(TARGET8): $(TARGET8_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(TARGET8_LIBS) $(OPTIONS)
	@echo "build $(TARGET8) ok"

install:
	@echo "nothing to install"

clean:
	rm -f *.o *.so *.a $(TARGET_LIST)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<



