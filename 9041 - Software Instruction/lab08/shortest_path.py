#!/usr/bin/python

import re,sys
from math import exp
import collections

start = sys.argv[1]
finish = sys.argv[2]

distance = collections.defaultdict(dict);

for line in sys.stdin.readlines():
	line = line.strip()
	line = re.sub(r'\s+',' ',line)
	array = line.split(' ')
	distance[array[0]][array[1]] = array[2]
	distance[array[1]][array[0]] = array[2]

shortest_journey = {}
shortest_journey[start] = 0
route = {}
route[start] = ""
unprocessed_towns = distance.copy()
current_town = start

while (current_town and current_town != finish):
	unprocessed_towns.pop(current_town, None)
	for town in unprocessed_towns.keys():
		if not distance.get(current_town, {}).get(town):continue
		d = int(shortest_journey[current_town]) + int(distance[current_town][town])
		if ((town in shortest_journey) and (shortest_journey[town] < d)):continue
		shortest_journey[town] = d
		route[town] = route[current_town] + " " +current_town

	min_distance = 100000;
	current_town = ""
	for town in unprocessed_towns.keys():
		if not town in shortest_journey: continue
		if shortest_journey[town] > min_distance:continue
		min_distance = shortest_journey[town]
		current_town = town


if not finish in shortest_journey:
	print("No route")
else:
	print("Shortest route is length = "+str(shortest_journey[finish])+":"+ route[finish] +" "+ finish +".")





