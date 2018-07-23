#!/bin/sh

for pngFile in "$@"
do
	read -p "Address to e-mail this image to? " Address
	read -p "Message to accompany image? " Message
	echo "$MESSAGE" | mutt -s "Test from COMP9041 Lab04" -a "$pngFile" -- "$Address"	
done
