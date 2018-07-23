#!/usr/bin/python

import re,sys,glob,math

lyDict = {}
allDict = {}
argvList = sys.argv[1:]


def getFrequency(array):
	final = {}
	for word in array:
		word = word.lower()
		for name in allDict.keys():
			result = 0
			#print(str(name), str(word))
			if word in allDict[name]:
				#print(str(name), str(word), str(allDict[name][word]))
				result = math.log(float(allDict[name][word] + 1) /float(lyDict[name]))
			else:
				result = math.log(1/float(lyDict[name]))

			if name in final:
				final[name] += result
			else:
				final[name] = result
	return final

def dataBase():
	for file in glob.glob("lyrics/*.txt"):
		total=0
		F = open(file, 'r')
		array = []
		for line in F:
			line = line.lower()
			words = re.split(r"[^a-zA-Z]", line)
			for word in words:
				pattern = re.compile("^$")
				if not pattern.match(word):
					total +=1
				array.append(word)
		name = file.replace(".txt", "")
		name = name.replace("_"," ")
		name = name.replace("lyrics/", "")
		F.close()
		lyDict[name] = total
		sDict = uniqDict(array)
		allDict[name] = sDict

def uniqDict(array):
	uniq = {}
	for word in array:
		word = word.lower()
		if word in uniq:
			uniq[word] += 1
		else:
			uniq[word] = 1
	return uniq

dataBase()

for f in argvList:
	F = open(f, 'r')
	array = []
	for line in F:
		line = line.lower()
		words = re.split(r"[^a-zA-Z]", line)
		for word in words:
			pattern = re.compile("^$")
			if not pattern.match(word):
				array.append(word)
	result = getFrequency(array)
	F.close()

	for key,value in sorted(result.items(), key=lambda x:(-x[1], x[0])): 
		print( str(f) + " most resembles the work of " + str(key) + " (log-probability=" + str(("%.1f" % value )) + ")" )
		break;
#print(allDict)
