
CXXFLAGS = -Werror -Wall -g -pipe -O2 -Wno-unused-variable -Wno-unused-function
CFLAGS = -Werror -Wall -g -pipe -O2 -Wno-unused-variable -Wno-unused-function

INCLUDE = -I. -I./plthook
LIBPATH = -L.
LIBS = -ltesta -ltesta_hook -lplthook -ldl 
TARGET3_LIBS = -ltesta -ltesta_hook -ldl 

TARGET1 = demo1
TARGET1_OBJS = demo1.o

TARGET2 = demo2
TARGET2_OBJS = demo2.o

TARGET3 = demo3
TARGET3_OBJS = demo3.o

TARGET4 = demo4
TARGET4_OBJS = demo4.o

.PHONY: clean plthook

all: testa testa_hook plthook $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

testa:
	$(CXX) $(CFLAGS) testa.c -fPIC -shared -o libtesta.so

testa_hook:
	$(CXX) $(CFLAGS) testa_hook.c -fPIC -shared -o libtesta_hook.so

plthook:
	$(CXX) $(CFLAGS) plthook/plthook_elf.c -fPIC -shared -o libplthook.so

$(TARGET1): $(TARGET1_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS)
	@echo "build $(TARGET1) ok"

$(TARGET2): $(TARGET2_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS)
	@echo "build $(TARGET2) ok"

$(TARGET3): $(TARGET3_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(TARGET3_LIBS)
	@echo "build $(TARGET3) ok"

$(TARGET4): $(TARGET4_OBJS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBPATH) $(LIBS)
	@echo "build $(TARGET4) ok"

install:
	@echo "nothing to install"

clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) *.o *.so

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<



