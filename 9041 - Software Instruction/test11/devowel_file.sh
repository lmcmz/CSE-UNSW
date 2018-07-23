#!/bin/bash

array=()

temp="$$_temp.txt"

cat $1 | while read line;
do
 echo $line |sed 's/[aeiouAEIOU]//g' >> $temp
done

rm $1
mv $temp $1
