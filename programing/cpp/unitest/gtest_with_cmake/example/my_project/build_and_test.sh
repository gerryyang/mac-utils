#!/bin/bash

if [ "$1" == "clean" ]; then
    rm -rf build
    exit 0
fi

cmake -S . -B build

cmake --build build

cd build && ctest