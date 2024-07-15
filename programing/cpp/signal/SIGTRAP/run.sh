#!/bin/bash

# 设置忽略 SIGTRAP 信号
trap "" TRAP

# 运行要忽略 SIGTRAP 信号的程序
./a.out