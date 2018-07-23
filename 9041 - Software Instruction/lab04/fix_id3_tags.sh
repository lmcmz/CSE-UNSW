#!/bin/bash

Dir=$PWD

for dir in "$@"
do
	cd "$dir"
	for file in *.mp3
	do
        	fileName=`echo $file | tr '-' '%' |sed 's/\ %\ /-/g' | sed 's/\ /_/g'`
        	Track=`echo $fileName | cut -f1 -d"-"`
        	Track=`expr $Track`
        	Title=`echo $fileName | cut -f2 -d"-" | tr '_' ' '| tr '%' '-'`
        	Artist=`echo $fileName | cut -f3 -d"-" | tr '_' ' '| tr '%' '-' | sed 's/.mp3$//g'`
        	Dirname=`echo $PWD | rev | cut -d"/" -f1 | rev`
        	Album=`echo "$Dirname"`
        	Year=`echo "$Dirname" | cut -d"," -f2 |sed 's/[ \t]*$//g'| sed 's/^[ \t]*//g'`
        	id3 -T $Track  -t "$Title" -a "$Artist" -A "$Album" -y "$Year" "$file" >/dev/null
	done
	cd ~
	cd $Dir
done
