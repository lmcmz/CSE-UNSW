#!/usr/bin/python

import sys


if (len(sys.argv)!=3):
	print "Usage: ./echon.py <number of lines> <string>"
	exit()

number=sys.argv[1]
if not number.isdigit() and number > 0:
	print "./echon.py: argument 1 must be a non-negative integer"
	exit()
string=sys.argv[2]
for i in range(0,int(number)):
	print string
	
