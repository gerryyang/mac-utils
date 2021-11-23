#!/bin/bash

rm -rf build
mkdir -p build
cd build

cmake -j8 .. 

/usr/bin/time -f "real %e user %U sys %S" make -j8 VERBOSE=1 TaskA

echo "have done"
