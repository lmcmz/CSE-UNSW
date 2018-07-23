#!/bin/bash

#set -x

#for file in *.jpg
#do
	#echo $file
	#if test `basename $file | egrep '\ '`
	#then
	#stupidName="$file $stupidName"
	#mv "$file" "`echo $file | tr '\ ' '_'`";
	#fi
#done

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
	#convert "$jpgFile" "$pngFile"
	#rm $jpgFile
done

#for file in *.png
#do
	#if test `basename $file|egrep stupidName`
	#then
	#mv "$file" "`echo $file | tr '_' '\ '`";
	#fi
#done
