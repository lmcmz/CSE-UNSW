#!/usr/bin/perl -w

@input = <STDIN>;
$words = 0;

sub removeEmptyStr
{
	$count = 0;
	foreach $str (@_)
	{
		chomp $str;
		if ($str eq "" or $str =~ /^\s+$/)
		{
			$count = $count + 0;	
			next;
		}
		$count++;
	}
	return $count;	
}

foreach $line (@input)
{
	chomp $line;
	$line =~ s/\s+/ /;
	if ($line eq "" or $line=~/^ *$/ or $line =~ /^\s*$/) {
		next;
	}
	@newLine = split(/[^a-zA-Z]+/, $line);	
	$words += removeEmptyStr(@newLine);
}

print "$words words\n";

