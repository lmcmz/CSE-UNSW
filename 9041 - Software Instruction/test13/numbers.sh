#!/bin/bash

start_num=$1
end_num=$2
filename=$3

while(true)
do
echo "$start_num" >> $filename
start_num=$((start_num + 1))
if [ $start_num -gt $end_num ]
then
break
fi
done
