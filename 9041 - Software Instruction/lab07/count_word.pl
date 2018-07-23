#!/usr/bin/perl -w

@input = <STDIN>;
$word = $ARGV[0];
$times = 0;

sub countStr
{
	$count = 0;
	foreach $str (@_)
	{
		chomp $str;
		$str=lc($str);
		if ($str eq $word)
		{
			$count++;
		}
	}
	return $count;	
}

foreach $line (@input)
{
	chomp $line;
	@newLine = split(/[^a-zA-Z]+/, $line);	
	$times += countStr(@newLine);
}

print "$word occurred $times times\n";

