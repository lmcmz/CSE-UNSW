#!/usr/bin/env python

import numpy as np
np.set_printoptions(threshold=np.nan)

rows  = 64
cols = 64
low = 10
high = 127

matrix = np.random.choice([x for x in range(low,high)],rows*cols)
matrix.resize(rows,cols)

filename="test.txt"
f = open(filename,'w')
f.write("%s" % matrix)
f.close()
print(matrix)
