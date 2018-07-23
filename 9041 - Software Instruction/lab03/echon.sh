#!/bin/sh

if test $# != 2
then
    echo "Usage: ./echon.sh <number of lines> <string>"
    exit 1
fi

if echo "$1" | grep -vq '^[0-9]*$'; 
then  
    echo "./echon.sh: argument 1 must be a non-negative integer" 
    exit 1 
fi  

for ((i=0; i < $1 ; i++))
do

echo $2

done
