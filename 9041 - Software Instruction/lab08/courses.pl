#!/usr/bin/perl -w

use List::MoreUtils qw(uniq);

$course=$ARGV[0];
$url = "http://www.timetable.unsw.edu.au/current/${course}KENS.html";

open F, "wget -q -O- $url |" or die;

while ($line = <F>) {
	my @array;
	if ($line =~ /$course/) {
		$line =~ m/>($course[0-9]{4})</;
		if ($1) { print "$1\n"};
	}
}
