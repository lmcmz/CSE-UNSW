#!/usr/bin/python

import sys

original = sys.argv[1:]
setOrignal = list(set(original))
setOrignal.sort(key=original.index)

print(" ".join(setOrignal))
