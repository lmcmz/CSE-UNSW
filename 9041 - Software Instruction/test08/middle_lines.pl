#!/usr/bin/perl -w

use POSIX;

$file = $ARGV[0];

open(my $F, '<', $file) or die "Could not open file '$file' $!";

$count = 0;
@input = <$F>;
close $F;
chomp @input;
$flag = $#input%2;

foreach $line (@input) {
	if (!($flag)) {
		if ($count == ceil($#input/2)) { 
			print "$line\n";
		}
	} else {
		if ($count == (($#input-1)/2) or $count == (($#input-1)/2) + 1) {
			print "$line\n";
		}
	}
	$count++;
}
