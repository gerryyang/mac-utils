#!/bin/bash

function cleanup {
    echo "Cleaning up temporary files..."
    rm -f /tmp/some_temp_file
}

trap cleanup EXIT

echo "Creating temporary file..."
touch /tmp/some_temp_file

echo "Press Ctrl+C to exit or wait for 10 seconds..."
sleep 10