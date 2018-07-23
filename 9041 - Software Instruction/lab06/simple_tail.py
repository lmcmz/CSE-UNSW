#!/usr/bin/python


import fileinput,sys

number = 10

for file in sys.argv[1:]:

#filelist = fileinput.input()

	strList=[]
	for line in fileinput.input(file):
		strList.append(line)

	head = len(strList) - 10
	if head < 0 :
		head = 0

	for line in strList[head : len(strList)]:
		print line,

