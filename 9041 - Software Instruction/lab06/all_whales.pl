#!/usr/bin/perl -w

my @whales;
my @input=<STDIN>;
my @flag;

sub findWhale 
{
	my $pod=0;
	my $number=0;
	my @arg=@_;
	my $whale=$arg[0];

	foreach my $string (@whales)
	{
	    if ($string =~ /:$whale/) {
	                my($count, $name) = split /:/, $string;
			$pod++;
			$number = $number + $count;
		}
	}
	$whale=~ s/\n//;
	print "$whale observations: $pod pods, $number individuals\n";
}

foreach $string (@input)
{
	$string = lc($string);
	$string =~ s/s$//;
	$string =~ s/\s+$//;
	$string =~ s/(\s)\s*/$1/g;
	$string =~ s/ /:/;
	push @whales,$string;
	my ($number, $name) = split /:/,$string;
	if ($name ~~ @flag){
		next;
	} else {
		push @flag,$name;
	}
}

@sortFlag = sort(@flag);

foreach $f (@sortFlag)
{
	$f=~ s/\n//;
	findWhale($f);
}

