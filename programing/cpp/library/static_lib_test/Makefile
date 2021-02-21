# makefile

CC     = gcc
CXX    = g++
RANLIB = ranlib
AR     = ar
AROPT  = -scurv

CFLAGS = -Wall -g -Os -pipe -D_REENTRANT -Wno-unused-function

LIB_A  = liba.a
OBJS_A = staticliba.o
LIB_B  = libb.a
OBJS_B = staticlibb.o

LIBS = $(LIB_A) $(LIB_B)
OBJS = $(OBJS_A) $(OBJS_B) 

INCLUDE = -I.
LIBPATH = -L.

BIN_LIBS = -la -lb
BIN_OBJS = main.o
BIN_SVR  = demo

all: $(BIN_SVR) 

$(BIN_SVR): $(BIN_OBJS) $(LIB_A) $(LIB_B)
	$(CXX) -o $@ $^ $(LIBPATH) $(BIN_LIBS)
	@echo "build $(BIN_SVR) OK!"

lib: $(LIB_A) $(LIB_B)

$(LIB_A): $(OBJS_A)
	$(AR) $(AROPT) $@ $? 
	$(RANLIB) $@

$(LIB_B): $(OBJS_B)
	$(AR) $(AROPT) $@ $? 
	$(RANLIB) $@

install:
	@echo "nothing to install"
		
clean:
	rm -f $(OBJS) $(LIBS) $(BIN_OBJS) $(BIN_SVR)


# calc relation between files automatically
.%.d: %.cpp
	$(CC) $(INCLUDE) -MM $< > $@
	@$(CC) $(INCLUDE) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@
%.o: %.cpp 
	$(CXX) $(CFLAGS) $(INCLUDE) -c $<

.%.d: %.c
	$(CC) $(INCLUDE) -MM $< > $@
	@$(CC) $(INCLUDE) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

