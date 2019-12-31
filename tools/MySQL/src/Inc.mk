# Inc.mk
CC     = gcc
CXX    = g++
RANLIB = ranlib
AR     = ar
AROPT  = -scurv
CFLAGS = -Werror -g -Os -pipe -D_REENTRANT

MYSQL_PATH = /data/home/gerryyang/deps/mysql_tlinux_64

INCLUDE = -I$(MYSQL_PATH)/include
LIBPATH = -L$(MYSQL_PATH)/lib

DEP_LIBS = -lmysqlclient -lz -ldl
LIBS    = $(DEP_LIBS)

%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $<
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<



