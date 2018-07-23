#!/usr/bin/python

import sys,re

for string in sys.stdin.readlines():
	string = re.sub('[0-4]', '<' , string)
	string = re.sub('[6-9]', '>' , string)
	print string,
