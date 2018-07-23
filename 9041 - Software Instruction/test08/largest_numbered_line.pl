#!/usr/bin/perl -w

use Data::Dumper;

@original = <STDIN>;
@input = @original;
chomp(@input);
chomp(@original);

$max = -100000;
$number = 0;
foreach $line (@input) {
	$line =~ s/[^\d \.-]//g;
	$line =~ s/\s+\.//g;
	$line =~ s/--/-/g;
	$line =~ s/-\s+|-$/ /g;
	$line =~ s/\s+/ /g;
	$line =~ s/^\s+|\s$//g;
	$line =~ s/-\./-0./g;
	if ($line eq '') {
		next;
	}
	@data = split(' ',$line);
	foreach $value (@data) {
		if($value > $max) {
			$max = $value;
		}
	}
}

foreach $line (@input) {
	@data = split(' ',$line);
	foreach $value (@data) {
		if($value == $max) {
			print "$original[$number]\n";
		}
	}
	$number++;
}
#print "Max: $max\n";
