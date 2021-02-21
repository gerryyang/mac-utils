#!/bin/bash

# origin
hostname

# hook
FAKE_HOSTNAME=gerryyang.com LD_PRELOAD=./gethostname.so hostname
