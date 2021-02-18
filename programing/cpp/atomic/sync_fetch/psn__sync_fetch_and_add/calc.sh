#!/bin/bash

rm psn_out_*

CNT=$1
if [[ "$CNT" -lt "1" ]]; then
	CNT=1
fi
echo "$CNT" process


for ((i=0; i<$CNT; i++))
do
	./gen_psn >> psn_out_$i&
done

wait 

echo "total:"
wc psn_out_* -l

echo "result:"

sort psn_out_* | uniq -d | wc -l
