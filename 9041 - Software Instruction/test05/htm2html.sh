#!/bin/bash

for file in *.htm
do
	html="`echo "$file"|sed 's/.htm$/.html/g'`"
	if ls|fgrep -c "$html">/dev/null
	then
	        echo "$html exists"
		exit 1 
	fi
	mv "$file" "$html"												
done
