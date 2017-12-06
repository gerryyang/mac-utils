#!/bin/bash

docker images | grep none | awk -F' ' '{print $3}' | while read f; do docker rmi -f $f; done


docker rm -f $(docker ps -aq)
docker rmi -f $(docker images -q)

