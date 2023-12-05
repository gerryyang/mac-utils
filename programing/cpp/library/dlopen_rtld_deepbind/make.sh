#!/bin/bash

./make_libdep_so.sh && ./make_dynamic_so.sh && ./make_main.sh

echo "ok"
