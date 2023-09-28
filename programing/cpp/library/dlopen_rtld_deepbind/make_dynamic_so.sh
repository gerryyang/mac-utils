#!/bin/bash

gcc -shared -fPIC dynamic.c -Wl,-rpath,. -L. -ldep -o dynamic.so
