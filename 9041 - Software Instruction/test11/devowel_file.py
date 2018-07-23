#!/usr/bin/python

import re,sys

filename = sys.argv[1]

input_flush = open(filename,'r')
array = list()
for line in input_flush.readlines():
	line = re.sub('[aeiouAEIOU]', '', line)
	array.append(line)

f = open(filename, 'w')
for string in array:
	f.write("%s" % string)
f.close()
