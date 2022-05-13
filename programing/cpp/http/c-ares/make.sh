#!/bin/bash

g++ ares_gethostbyaddr.cc -o ares_gethostbyaddr -I deps/c-ares-1.18.1/include deps/c-ares-1.18.1/lib/libcares.a
g++ ares_gethostbyname.cc -o ares_gethostbyname -I deps/c-ares-1.18.1/include deps/c-ares-1.18.1/lib/libcares.a
g++ ares_gethostbyname2.cc -o ares_gethostbyname2 -I deps/c-ares-1.18.1/include deps/c-ares-1.18.1/lib/libcares.a
g++ -std=c++11 performance_compare.cc -o performance_compare -I deps/c-ares-1.18.1/include deps/c-ares-1.18.1/lib/libcares.a

