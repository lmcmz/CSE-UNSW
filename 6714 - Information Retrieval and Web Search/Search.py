#!/usr/bin/python

import random

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
			
	def dump(self):
		print("* {} : {} (cur = {})".format(self.data[:self.cur], self.data[self.cur:], self.cur))
		
	def peek(self, pos):
		# look at the element under the current cursor, but does not advance the cursor. 
		if pos < len(self.data):
			return self.data[pos]
		else:
			return None
	def reset(self):
		self.cur = 0 

def	naive_search(sequence, value):
	j = len(sequence) 
	for i,item in enumerate(sequence):
		if value < item and j == len(sequence):
			j = i
		if value == item:
			return i
	return j

def binary_search(sequence, value):
	j = len(sequence) 
	lo, hi = 0, len(sequence) - 1
	while lo <= hi:
		mid = (lo + hi) // 2
		if sequence[mid] < value:
			lo = mid + 1
		elif value < sequence[mid]:
			hi = mid - 1
			if j == len(sequence):
				j = mid
		else:
			return mid
	return j

#print(binary_search([2, 4, 6, 8, 10], 1))

def gallop_to(a, val):# do not change the heading of the function
	original = a.cur
	count = 0
	while not a.eol() and a.elem() < val:
		count = 2*count 
		if count == 0: 
			count = 1
		if count > (len(a.get_list()) - a.cur-1):
			count = len(a.get_list()) - a.cur
		a.cur += count 
	max_i = a.cur + 1
	min_i = a.cur - count
	a_list = a.get_list()
	#print(a_list[min_i:max_i],min_i,max_i)
	a.cur = binary_search(a_list[min_i:max_i], val) + min_i

def test_gallop(l, val):
#	print("=> gallop_to({})".format(val))
	l.reset()
	gallop_to(l,val)
	#l.dump()

a = InvertedList([2, 4, 6, 8, 10, 12, 14, 16, 18])
b = InvertedList([1, 2, 4, 8, 16, 32])

test_a = [val - 1 for val in a.get_list()]
test_a.append(a.get_list()[-1] + 100)

#a.cur = len(a.get_list())
#a.dump()
#for t_a in test_a:
#	test_gallop(a, t_a)



random.seed(1999)
a = InvertedList(sorted([random.randint(1,50) for _ in range(100)]))
print(a.get_list())
for b in (range(1,50)):
	gallop_to(a, b)
	print(a.cur)

def intersection_galloping(a, b):
	# just in case these lists have been traversed.
	a.reset()
	b.reset()
	count = 0

	ret = []
	while not a.eol() and not b.eol():
		if a.elem() == b.elem():
			ret.append(a.elem())
			a.next()  # Note that here you are only allowed to move the cursor of one InvertedList Object. 
		else:
			if a.elem() < b.elem():
				gallop_to(a,b.elem())
			else:
				gallop_to(b,a.elem())
	# end_while
	return ret

#print(intersection_galloping(a, b))
