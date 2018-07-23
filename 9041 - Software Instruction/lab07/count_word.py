#!/usr/bin/python

import re,sys

count = 0
key = sys.argv[1]

for line in sys.stdin:
	line = line.lower()
	words = re.split(r"[^a-zA-Z]", line)
	for word in words:
		if word == key:
			count += 1;


print (key + " occurred " + str(count) + " times")
