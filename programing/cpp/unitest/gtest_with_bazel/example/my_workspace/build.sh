#!/bin/bash

if [ "$1" == "clean" ]; then
    bazel clean --expunge_async
fi

bazel test --cxxopt=-std=c++14 --test_output=all //:hello_test