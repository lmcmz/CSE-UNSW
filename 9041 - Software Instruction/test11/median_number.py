#!/usr/bin/python3

import os,sys

array = sys.argv[1:]
array = [int(x) for x in array]
sorted_array = sorted(array)
print(sorted_array[len(sorted_array)//2])
