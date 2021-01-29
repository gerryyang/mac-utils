#!/bin/bash

rm -rf build
mkdir -p build
cd build

# 通过 DCMAKE_BUILD_TYPE 指定不同的编译版本
cmake -j8 .. -DCMAKE_BUILD_TYPE=Debug
#cmake -j8 .. -DCMAKE_BUILD_TYPE=Release

/usr/bin/time -f "real %e user %U sys %S" make -j8 VERBOSE=1

echo "have done"
