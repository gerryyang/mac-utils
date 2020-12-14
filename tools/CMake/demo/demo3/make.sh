#!/bin/bash

pushd lib
g++ calculator.cc -g -Wall -std=c++11 -c
ar -crv libcalculator.a calculator.o

popd
g++ main.cc -g -Wall -std=c++11 -o Exp2 -Llib -lcalculator

echo "have done"

