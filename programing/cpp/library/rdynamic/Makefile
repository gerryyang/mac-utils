.PHONY: all clean test

#LDEXTRAFLAGS ?= -rdynamic -Wl,--version-script=symbol.txt
LDEXTRAFLAGS ?= -rdynamic 

all: prog

bar.o: bar.cc
	$(CXX) -O0 -g -c -Wall -fpic -o $@ $<

libbar.so: bar.o
	$(CXX) -shared -o $@ $<

main.o: main.cc
	$(CXX) -O0 -g -c -Wall -o $@ $<

prog: main.o | libbar.so
	$(CXX) $(LDEXTRAFLAGS) -o $@ $< -ldl

clean:
	rm -f *.o *.so prog

test: prog
	./$<
