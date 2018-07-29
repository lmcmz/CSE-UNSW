#!/usr/bin/python

import math

inputs = "11110101111011"
b = 4


def decode_rice(inputs, b):
	code = list(inputs)
	number = math.ceil(math.log(b ,2))
	skiper = 0
	counter = 0
	result = []
	for i,item in enumerate(code):
		if i < skiper : continue
		if item == "1": 
			counter += 1
		else:
			skiper = i + number +1
			endList = code[i+1:i+number+1]
			endStr = ''.join(endList)
			result.append(b*counter +int(endStr,2))
			counter = 0
	return(result)
	
decode_rice(inputs, b)