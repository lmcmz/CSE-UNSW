#!/usr/bin/python

import sys,re

whales=[]
flag=set()

def findWhale(whale):
	pod = 0
	number = 0
	for line in whales:
		if re.search(":"+whale,line):
			strList = re.split(r":", line)
			pod = pod + 1
			number = number + int(strList[0])
	print whale + " observations: " + str(pod) + " pods, " + str(number) +" individuals"

for line in sys.stdin.readlines():
	line = line.lower()
	line = re.sub('s$','',line)
	line = re.sub('\s+',' ',line).strip()
	line = re.sub(' ',':',line, 1)
	whales.append(line)
	strList = re.split(r':',line)
	flag.add(strList[1])


flag = sorted(flag)

for f in flag:
#	f = re.sub('\n','',f)
	findWhale(f)
