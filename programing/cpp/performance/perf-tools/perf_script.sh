#!/bin/bash

perf script | awk '$2 ~ "__x64_sys" { print $2 }' | sort | uniq -c