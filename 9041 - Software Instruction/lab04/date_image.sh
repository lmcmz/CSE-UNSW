#!/bin/bash

for image in "$@"
do
	date=`stat "$image" |egrep "Modify"|cut -d" " -f2-4`
	#date=`ls -l "$image" | egrep '.jpg$|.png$'| cut -d" " -f6-8`
	convert -gravity south -pointsize 36 -draw "text 0,10 '$date'" "$image" "$image"
	touch -d "$date" "$image"
done
