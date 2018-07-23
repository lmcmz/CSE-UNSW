#!/usr/bin/python

import sys,re

whale=sys.argv[1]
pod=0
number=0
for string in sys.stdin.readlines():
	if re.search(whale,string):
		strList = re.split(r"\s+",string)
		pod = pod + 1
		number = number + int(strList[0])

print whale + " observations: " + str(pod) + " pods, " + str(number) +" individuals"
