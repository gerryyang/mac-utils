#!/bin/bash

pushd lib
g++ calculator.cc -g -Wall -std=c++11 -c -fPIC
g++ calculator.o -g -Wall -std=c++11 -shared -o libcalculator.so

## or 
#g++ calculator.cc -g -Wall -std=c++11 -shared -fPIC -o libcalculator.so

popd
g++ main.cc -g -Wall -std=c++11 -o Exp2 -Llib -lcalculator

echo "have done"

# export LD_LIBRARY_PATH=lib
