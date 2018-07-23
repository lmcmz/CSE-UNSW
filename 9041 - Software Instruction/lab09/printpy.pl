#!/usr/bin/perl -w

my $line;
foreach $var (@ARGV) {
	$line .= $var;
}
$line =~ s/\\/\\\\/g;
$line =~ s/\"/\\"/g;
$line =~ s/\n/\\n/g;
#print "$line\n";
print "print(\"$line\")";
