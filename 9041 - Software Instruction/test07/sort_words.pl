#!/usr/bin/perl -w

while ($line =<>) {
	 print join(' ', sort(split(' ', $line))), "\n";
}
