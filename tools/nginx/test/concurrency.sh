#!/bin/bash

clinum=10
i=1

echo "concurrency test begin"
while [ "$i" -le "$clinum" ]; do
	echo "start cli $i"
	sh test.sh &
	i=`expr "$i" + 1`
done
echo "concurrency test end"

