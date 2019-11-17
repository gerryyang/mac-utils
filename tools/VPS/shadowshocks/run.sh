#!/bin/bash

echo "start ss"
sudo ssserver -p 443 -k wcdj12948 -m aes-256-cfb --user nobody -d start
