#!/usr/bin/python3

i=1
while i<=9:
	j=1
	while j<=i:
		mut =j*i
		print("%d*%d=%d"%(j,i,mut), end="  ")
		j+=1
	print("")
	i+=1