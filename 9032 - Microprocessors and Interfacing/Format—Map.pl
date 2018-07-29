#!/usr/bin/perl

my $string;
foreach $line (<>) {
	$line =~ s/\[//g;
	$line =~ s/\n//g;
	#$line =~ s/$//g;
	#$line =~ s/\]/]\n/g;
	$agr = join(',', split(' ',$line));
	$agr =~ s/$/,/g;
	$string .= $agr;
}
$string =~ s/\n//g;
$string =~ s/\]/\n.db/g;
$string =~ s/\.db,/.db /g;
print "$string\n";