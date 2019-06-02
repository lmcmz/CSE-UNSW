#!/bin/bash

# Play agent against specified program
# Example:
# ./playc.sh lookt 12345

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <player> <port>" >&2
  exit 1
fi

./servt -p $2 & sleep 0.1
./agent -p $2 & sleep 0.1
./$1    -p $2
