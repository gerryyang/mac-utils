#!/bin/bash

gcc main.c -Wl,-rpath,. -L. -ldep -ldl
