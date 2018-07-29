#!/usr/bin/python

import random,math

random.seed(1999)
n = 30
index = [random.randint(1, 100) for _ in range(n)]

def Logarithmic_merge(index, cut_off, buffer_size):
	data = index[:cut_off]
	length = len(data)
	reminder = length%buffer_size
	number = math.floor(length/buffer_size)	
	layer = math.floor(math.log(number, 2))
	subLists = []
	for i in range(1,number+1):
		subList = data[buffer_size*(i-1):buffer_size*i]
		subLists.append(subList)
	reminderList = data[number*buffer_size:length]
	#print(subLists)
	
	while(check_continue(subLists)):
		for i,item_1 in enumerate(subLists):
			for j,item_2 in enumerate(subLists):
				if i == j : continue; 
				if len(item_1) == len(item_2) and len(item_1) is not 0:
					new_list = merge_list(item_1, item_2)
					subLists.remove(item_1)
					subLists.remove(item_2)
					subLists.insert(i, new_list)
					break
	#print(subLists)
	finalList = make_final_list(subLists, layer, buffer_size)
	finalList.insert(0, reminderList)
	return(finalList)

					
	
	'''
	for i in range(1,layer+1):
		for j in range(0,int(number/2)):
			list_1 = subLists.pop(0)
			if not subLists : 
				subLists.append(list_1)
				subLists.insert(0, [])
				break;
			list_2 = subLists.pop(0)
			new_list = sorted(list_1+list_2)
			subLists.append(new_list)
	print(subLists)
	finalList = make_final_list(subLists, layer, buffer_size)
	finalList.insert(0, reminderList)
	return(finalList)
	'''

def make_final_list(array,number,buffer_size):
	template = [ [] for _ in range(number+1)]
	for item in array:
		for i in range(-1,number):
			if len(item) == buffer_size*(2**(i+1)):
				template[i+1] = sorted(item)
	return template
	
def merge_list(list_1,list_2):
	return sorted(list_1+list_2)

'''
def insert_emptyList(array,number,buffer_size):
	for item in array:
		for i in range(0,number):
			if len(item) is not buffer_size*(i+1):
				array.insert(0,[])
	return array
'''


def check_continue(array):
	for i,item_1 in enumerate(array):
		for j,item_2 in enumerate(array):
			if i == j : continue;
			if (len(item_1) == len(item_2) and len(item_1) is not 0):
				return True
			else:
				return False
	return False
	
for i in range(10,16):
	for j in range(1,5):
		print(Logarithmic_merge(index, i, j))