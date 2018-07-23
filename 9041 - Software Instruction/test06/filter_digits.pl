#!/usr/bin/perl -w

@input=<STDIN>;

foreach my $line (@input)
{
	$line =~ s/[0-9]+,?[0-9]*//g;
	print $line;
}
