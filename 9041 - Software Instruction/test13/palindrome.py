#!/usr/bin/python
import re,sys

string = sys.argv[1]
string = re.sub('\W', "", string)
string = string.lower()
reverse_string = "".join(reversed(string))
if reverse_string == string:
	print("True")
else:
	print("False")

