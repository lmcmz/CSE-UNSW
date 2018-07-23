#!/usr/bin/perl -w

@input=<STDIN>;
$whale=$ARGV[0];

$pod=0;
$number=0;
foreach my $string (@input)
{
	if ($string =~ /$whale/) {
		my($count, $name) = split / /, $string;
		$pod++;
		$number = $number + $count;
	}
}

print "$whale observations: $pod pods, $number individuals\n";
