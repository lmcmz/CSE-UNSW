#!/usr/bin/python3

import sys,re

def findFrequency(word_list,char):
	count = 0
	for i in word_list:
		if (char.lower() == i.lower()):
			count += 1
	return count

for line in sys.stdin.readlines():
	words_list = re.split('\s+',line)
	line_count = 0
	for word in words_list:
		line_count += 1
		number = 0;
		count = 0;
		for char in word:
			count += 1;
			if (len(word) == 1):
				if (line_count == len(words_list)):
					print(word, end='\n')
				else: 
					print(word, end=' ')
				break
				
			if (number == 0):
				number = findFrequency(word,char)
				
			if (count == len(word) and number == findFrequency(word,char)):
				if (line_count == len(words_list)):
					print(word, end='\n')
				else:
					print(word, end=' ')
			if (number != findFrequency(word,char)):
				break
	print("")
		