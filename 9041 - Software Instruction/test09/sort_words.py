#!/usr/bin/python

import sys

for line in sys.stdin.readlines():
	array = line.split()
	array = sorted(array)
	print(' '.join(array))
