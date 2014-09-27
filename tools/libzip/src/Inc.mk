# Inc.mk

CC = gcc
CXX = g++
RANLIB = ranlib
AR = ar
AROPT = -scurv

CFLAGS = -Werror -g -Os -pipe -D_REENTRANT
#CFLAGS = -Wall -g -Os -pipe -D_REENTRANT

ZLIB_PATH = /Users/gerryyang/LAMP/zlib/install/zlib-1.2.8
LIBZIP_PATH = /Users/gerryyang/LAMP/libzip/install/libzip-0.11.2

INCLUDE = -I$(ZLIB_PATH)/include -I$(LIBZIP_PATH)/include -I$(LIBZIP_PATH)/lib/libzip/include
LIBPATH = -L$(ZLIB_PATH)/lib -L$(LIBZIP_PATH)/lib

DEP_LIBS = -lzip -lz
LIBS = $(DEP_LIBS)


# calc relation between files automatically
%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

