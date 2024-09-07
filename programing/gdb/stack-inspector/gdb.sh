#!/bin/bash

gdb -q -ex "b func1" \
    -ex "run" \
    -ex "bt" \
    -ex "source ./stack-inspector.py" \
    -ex "stack-inspector" \
    --args ./test