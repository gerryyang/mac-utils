# makefile

#CC     = gcc-4.8
#CXX    = g++-4.8

#CC     = gcc-8
CXX    = g++-8

RANLIB = ranlib
AR     = ar
AROPT  = -scrv

CXXFLAGS = -std=c++11 -Wall -g -Os -pipe -D_GLIBCXX_USE_CXX11_ABI=0 #-Wabi-tag 
CFLAGS = -Wall -g -Os -pipe 

LIB = libhello.a
LIB_OBJS = hello.o

INCLUDE = -I.
LIBPATH = -L.

BIN_LIBS = -lhello
BIN_OBJS = demo.o
BIN  = demo

all: $(BIN) 

$(BIN): $(BIN_OBJS)
	$(CXX) -o $@ $^ $(LIBPATH) $(BIN_LIBS)
	@echo "build $(BIN) OK!"

lib: $(LIB)

$(LIB): $(LIB_OBJS)
	$(AR) $(AROPT) $@ $? 
	$(RANLIB) $@
		
clean:
	rm -f $(LIB_OBJS) $(LIB) $(BIN_OBJS) $(BIN)


.%.d: %.cpp
	$(CC) $(INCLUDE) -MM $< > $@
%.o: %.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

.%.d: %.c
	$(CC) $(INCLUDE) -MM $< > $@
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

