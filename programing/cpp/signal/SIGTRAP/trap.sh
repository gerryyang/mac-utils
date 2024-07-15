#!/bin/bash

trap "echo 'Caught SIGINT signal. Exiting...'; exit 1" INT

echo "Press Ctrl+C to exit..."
while true; do
    sleep 1
done