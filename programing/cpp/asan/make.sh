#!/bin/bash

clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer test.cc -o test
clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer test2.cc -o test2
clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer test3.cc -o test3