#!/bin/bash

if [ "$1" == "clean" ]; then
    bazel clean --expunge_async
    exit 0
fi

bazel test --cxxopt=-std=c++14 --test_output=all //:hello_test