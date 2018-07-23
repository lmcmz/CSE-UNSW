#!/bin/bash

for file in "$@"
do
	headfile=`cat "$file" | egrep "#include\ *\"" | sed -E 's/^#include "//' | sed -E 's/"$//'`
	for head in $headfile
	do
		newHead=`echo "$head" | sed 's/\ //g'`
		if test -e "$newHead"
		then
			continue
		else
			echo "$head included into $file does not exist"
		fi
	done
done
