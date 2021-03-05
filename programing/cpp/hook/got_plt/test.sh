#!/bin/bash

echo "test begin..."
echo ""

for i in {1..5}
do
	echo "test demo$i"
	./demo"$i"
	echo ""
done

echo "test end..."
