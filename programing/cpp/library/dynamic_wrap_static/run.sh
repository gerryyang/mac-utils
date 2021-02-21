#!/bin/sh
set -e

gcc -Wall -g -c -o libfunc-static.o libfunc.c -fPIC
ar rcs libfunc-static.a libfunc-static.o

gcc -fPIC -Wall -g -c libfunc_wrap.c
gcc -g -shared -Wl,-soname,libfunc_wrap.so.0 -o libfunc_wrap.so.0.0 libfunc_wrap.o -lc -L./ -lfunc-static

gcc -Wall -g -c main.c -o main.o
/sbin/ldconfig -n .
ln -sf libfunc_wrap.so.0 libfunc_wrap.so
gcc -g -o main main.o -L. -lfunc_wrap
LD_LIBRARY_PATH="." ./main


