#!/usr/bin/perl -w

$number = $ARGV[0];
$file = $ARGV[1];


open(my $F, '<', $file) or die "$0: Can't open $file: $!\n";
my @Array;

while(<$F>){
push @Array, $_;
}

if (($number -1) <= $#Array && ($number -1) >=0){
	print $Array[$number -1];
}
