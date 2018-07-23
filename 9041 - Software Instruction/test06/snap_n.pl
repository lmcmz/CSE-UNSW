#!/usr/bin/perl -w

$number = $ARGV[0];
@input = <STDIN>;

foreach $line (@input) {
	$counter=0;
	foreach $string (@input) {
		if ($string eq $line) {
			$counter++;
		}
	}
	if ($counter eq $number)
	{
		print "Snap: $line";
		exit 0;
	}
}
