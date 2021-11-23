.PHONY: all clean test

LDEXTRAFLAGS ?= -rdynamic -Wl,--version-script=symbol.txt
#LDEXTRAFLAGS ?= -rdynamic 

all: prog

bar.o: bar.c
	gcc -c -Wall -fpic -o $@ $<

libbar.so: bar.o
	gcc -shared -o $@ $<

main.o: main.c
	gcc -c -Wall -o $@ $<

prog: main.o | libbar.so
	#gcc $(LDEXTRAFLAGS) -o $@ $< -L. -lbar -ldl
	gcc $(LDEXTRAFLAGS) -o $@ $< -ldl

clean:
	rm -f *.o *.so prog

test: prog
	./$<
