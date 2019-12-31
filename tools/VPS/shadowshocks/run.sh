#!/bin/bash

echo "start ss"
sudo ssserver -p 443 -k wcdj***** -m aes-256-cfb --user nobody -d start
