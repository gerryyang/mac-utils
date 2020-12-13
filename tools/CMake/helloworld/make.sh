#!/bin/bash

## https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake

rm -rf build
mkdir -p build && cd build


#COMPILE_MODE=gcc
COMPILE_MODE=clang

## use `ccmake .` to use cmake gui
if [ $COMPILE_MODE == "gcc"  ]; then
	export CC=/usr/bin/cc
	export CXX=/usr/bin/c++

	cmake ..

elif [ $COMPILE_MODE == "clang" ]; then

	export CC=/root/compile/llvm_install/bin/clang
	export CXX=/root/compile/llvm_install/bin/clang++
	
	cmake -fuse-ld=lld  -DCMAKE_USER_MAKE_RULES_OVERRIDE=./ClangOverrides.txt  ..
	#cmake -fuse-ld=lld  -DCMAKE_TOOLCHAIN_FILE=./LinuxToolchains.cmake  ..

else
	echo "error: $COMPILE_MODE invalid"
	exit 1
fi

make -j VERBOSE=1

## LLD leaves its name and version number to a .comment section in an output
## readelf --string-dump .comment <output-file>

echo "have done"

