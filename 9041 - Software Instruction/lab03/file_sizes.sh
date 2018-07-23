#!/bin/sh

small=""
medium=""
large=""

for filename in `ls | egrep -v '\.sh$'| tr " " "\n"` 
do
	if test  `cat $filename | wc -l` -lt 10
	then
	small="$small $filename"
        continue
        fi

        if test  `cat $filename | wc -l` -lt 100
	then
	medium="$medium $filename"
	continue
        fi

	large="$large $filename"
done

echo "Small files:$small"
echo "Medium-sized files:$medium"
echo "Large files:$large"
