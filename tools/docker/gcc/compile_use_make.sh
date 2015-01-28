#!/bin/bash

NAME=hello
SRC_FILE=hello.c

docker run --rm -v /root/Docker/gcc/src:/usr/src/myapp:rw -w /usr/src/myapp gcc_latest_gerry:demo gcc -o "$NAME" "$SRC_FILE"
