#!/bin/bash

g++ -std=c++11 -shared -g -fPIC -o libtest.so test.cpp
g++ -std=c++11 main.cpp -ldl
