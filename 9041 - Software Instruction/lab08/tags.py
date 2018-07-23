#!/usr/bin/python

import re,sys
import urllib

url = sys.argv[len(sys.argv) - 1]

response = urllib.urlopen(url)

html = response.read().decode('utf-8')
html = html.strip()
html = html.lower()

regex = re.compile(r'<(\w+\d*)\ ?', re.I)
dict_tag = {}
for tag in regex.findall(html):
	if tag in dict_tag:
		dict_tag[tag] += 1
	else:
		dict_tag[tag] = 1;

if (len(sys.argv) == 3 and sys.argv[1] == "-f"):
	for key,value in sorted(dict_tag.items(), key=lambda x: (x[1],x[0])):
		print(key + " " +str(value))
else:
	for key in sorted(dict_tag):
		print(key + " " +str(dict_tag[key]))
