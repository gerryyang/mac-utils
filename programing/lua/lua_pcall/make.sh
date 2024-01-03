#!/bin/bash

# lua 5.3.5
g++ -g -I../lua_5.3.5/include -L../lua_5.3.5/lib lua_pcall.cc -llua  -ldl
echo "done"
