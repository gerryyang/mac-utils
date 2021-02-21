#!/bin/bash

gcc -o fpic-no-pic.s -S fpic.c
gcc -fPIC -o fpic-pic.s -S fpic.c

gcc -shared -o fpic-no-pic.so fpic.c
gcc -shared -fPIC -o fpic-pic.so fpic.c
