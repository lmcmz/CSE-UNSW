#!/usr/bin/perl -l

my $scalar = @ARGV;
my $number = @ARGV[0];
my $string = @ARGV[1];

if ($scalar != 2) {
	print "Usage: ./echon.pl <number of lines> <string>";
	exit 1;
}

if ($number !~ /^[0-9]*$/) {
	print "./echon.pl: argument 1 must be a non-negative integer";
	exit 1;
}

foreach my $i(1..$number){
	print "$string";
}
