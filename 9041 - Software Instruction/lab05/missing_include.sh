#!/bin/bash

for file in "$@"
do
	headfile=`cat "$file" | egrep "#include\ *\"" | sed -E 's/^#include "//' | sed -E 's/"$//'`
	for head in $headfile
	do
		if ls|fgrep -c "$head">dev>null
		then
			echo "$head included into $file does not exist"
		fi
	done
done
