#!/bin/bash

PB_PATH=/home/ubuntu/tools/protobuf-3.15.8-install
g++ -std=c++11 -g -o read_msg -I../src_protocol -I$PB_PATH/include -L$PB_PATH/lib -lprotobuf read_msg.cc


