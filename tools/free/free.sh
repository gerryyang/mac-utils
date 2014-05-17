#!/bin/bash

sysctl -a | grep "mem" | grep "hw.memsize"

echo "-------------------"
vm_stat

# or use top command

