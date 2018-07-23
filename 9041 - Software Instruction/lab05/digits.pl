#!/usr/bin/perl

@string=<STDIN>;

foreach my $line ( @string ) {
	$line=~tr/[0-4]/</;
	$line=~tr/[6-9]/>/;
}
print @string;
