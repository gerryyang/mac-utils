#!/bin/bash

# start apache
#exec apache2 -D FOREGROUND

# start golang app
/app/src/http/http_svr_v1 &

while true; do
    sleep 1
done

