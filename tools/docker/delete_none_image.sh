#!/bin/bash

docker images | grep none | awk -F' ' '{print $3}' | while read f; do docker rmi -f $f; done
