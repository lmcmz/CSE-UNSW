#!/usr/bin/perl -w

$file = $ARGV[0];
open(my $fh, '<', $file) or die "Could not open file '$file' $!";
my @array;
foreach $line (<>) {
 $line =~ s/[aeiouAEIOU]//g;
 push(@array, $line);
}

open(my $f, '>', $file);
print $f @array;
close($f)
