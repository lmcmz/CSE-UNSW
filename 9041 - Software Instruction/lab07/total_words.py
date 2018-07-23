#!/usr/bin/python

import re,sys

count = 0
for line in sys.stdin:
	words = re.split(r"[^a-zA-Z]", line)
	for word in words:
		pattern = re.compile("^$")
		if not pattern.match(word):
				count += 1



print str(count) + " words"
