#!/usr/bin/perl -w

use Data::Dumper;

@input=<STDIN>;

my %whales;
foreach $string (@input)
{
	$string = lc($string);
	$string =~ s/s$//;
	$string =~ s/(\s)\s*/$1/g;
	$string =~ s/ /:/;
	my ($number, $name) = split /:/,$string;
	if ($whales{$name}){
		$whales{$name} = $whales{$name} + $number;
	} else {
		$whales{$name} = $number;
	}
}

print Dumper(\%whales);

foreach my $name (sort keys %whales) 
{	
	print "$name observations: $whales{$name} individuals\n";
}
