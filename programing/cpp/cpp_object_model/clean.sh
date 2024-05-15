#!/bin/bash

find . -type f -name "test[0-9]*" ! -name "*.cpp" -exec rm {} \;