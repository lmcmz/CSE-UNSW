#!/bin/bash

for jpgFile in *.jpg
do
	pngFile="`echo "$jpgFile" | sed 's/.jpg$/.png/g'`"
	if ls|fgrep -c "$pngFile">/dev/null
	then
		echo "$pngFile already exists"
	else
		convert "$jpgFile" "$pngFile"
		rm "$jpgFile"
	fi
done

