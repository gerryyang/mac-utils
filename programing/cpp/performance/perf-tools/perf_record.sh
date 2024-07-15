#!/bin/bash

perf record -p `pidof gamesvr` -c 1 -e cs -g -- sleep 10