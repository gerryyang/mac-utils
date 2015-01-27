#!/bin/bash

# start ssh
/usr/sbin/sshd &

# start golang app
/app/src/socket/tcpechov1/tcpsvr &

while true; do
    sleep 1
done

