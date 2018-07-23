#!/usr/bin/python

import sys

number = sys.argv[1]

setLine = set()
finalNumber = 0

for line in sys.stdin.readlines():
	line = line.lower()
	array = line.split()
	line = ' '.join(array)
	setLine.add(line)
	finalNumber += 1
	if int(number) == len(setLine):
		break

if int(number) > len(setLine):
	print("End of input reached after {} lines read -  {} different lines not seen.".format(finalNumber,number))
else:
	print("{} distinct lines seen after {} lines read.".format(len(setLine),finalNumber))
