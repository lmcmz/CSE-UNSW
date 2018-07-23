#!/usr/bin/perl -w

@Array=<STDIN>;

my $temp;
foreach $arg (@Array){
	$randon = int(rand($#Array));
	$temp = $Array[$randon];
	$Array[$randon] = $arg;
	$arg = $temp;
}

print @Array;
