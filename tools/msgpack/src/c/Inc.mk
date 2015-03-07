# Inc.mk

CC = gcc
CXX = g++
RANLIB = ranlib
AR = ar
AROPT = -scurv

CFLAGS = -Wall -g -Os -pipe 

MSGPACK_PATH = /Users/gerryyang/LAMP/msgpack/install/msgpack-0.5.5

INCLUDE = -I$(MSGPACK_PATH)/include
LIBPATH = -L$(MSGPACK_PATH)/lib

DEP_LIBS = -lmsgpack 
LIBS = $(DEP_LIBS)


# calc relation between files automatically
%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

