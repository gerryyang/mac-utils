#!/bin/bash

# start ssh service
/usr/sbin/sshd &

# start apache service
exec apache2 -D FOREGROUND
