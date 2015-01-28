#!/bin/bash

# --rm  Remove intermediate containers after a successful build
docker build --rm -t gcc_latest_gerry:demo .
