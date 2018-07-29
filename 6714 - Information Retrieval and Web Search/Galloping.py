#!/usr/bin/python

import math

class InvertedList:
	def __init__(self, l):
		self.data = l[:] # make a copy
		self.cur = 0     # the cursor 

	def get_list(self):
		return self.data
 
	def eol(self):
		# we use cur == len(list) to indicate EOL
		return False if self.cur < len(self.data) else True
	
	def next(self, val = 1):
		# does not allow cur to be out-of-range, but use len(list) to indicate EOL
		self.cur = min(self.cur + val, len(self.data)) 
			
	def elem(self):
		if self.eol():
			return None
		else: 
			return self.data[self.cur]
	def peek(self, pos):
		# look at the element under the current cursor, but does not advance the cursor. 
		if pos < len(self.data):
			return self.data[pos]
		else:
			return None
	def reset(self):
		self.cur = 0   


a = InvertedList([2, 4, 6, 8, 10, 12, 14, 16, 18])
b = InvertedList([1, 2, 4, 8, 16, 32])

	
def gallop_to(a, val):
	counter = 0
	if a.elem() >= val : 
#		a.cur = original
		return counter
#	a.cur = 1
	i = 1
	while not a.eol() and a.elem() <= val:
		a.cur += 2*i
		counter += 1
		i += 1
	a.cur -= 2*i 
	#a.next()
	return counter

def intersection_galloping(a, b):
	# just in case these lists have been traversed.
	a.reset()
	b.reset()
	count = 0

	ret = []
	while not a.eol() and not b.eol():
		print(a.elem(), b.elem())
		if a.elem() == b.elem():
			ret.append(a.elem())
			a.next()  # Note that here you are only allowed to move the cursor of one InvertedList Object. 
		else:
			if a.elem() < b.elem():
				count += gallop_to(a,b.elem())
			else:
				count += gallop_to(b,a.elem())
	# end_while
	print(ret, count)
	return ret
	
intersection_galloping(a, b)	



