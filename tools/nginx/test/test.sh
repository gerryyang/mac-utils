#!/bin/bash

reqnum=100
i=1

echo "test begin"
while [ "$i" -le "$reqnum" ]; do
	curl "http://127.0.0.1/index.html" > /dev/null
	if [ $? != 0 ]; then
		echo "curl err"
	else
		echo "curl ok"
	fi

	i=`expr "$i" + 1`
done
echo "test end"
