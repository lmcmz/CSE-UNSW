#!/usr/bin/perl

@array = sort { $a <=> $b } (@ARGV);
print "$array[int($#array/2)]\n";
