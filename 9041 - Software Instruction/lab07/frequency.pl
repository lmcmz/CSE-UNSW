#!/usr/bin/perl -w

$word = $ARGV[0];

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

foreach $file (glob "lyrics/*.txt") 
{
	open(my $f, "<", $file) 
		or die "cannot open < input.txt: $!";
	my @input;
	while(<$f>){
		push @input, $_;
	}
	frequency(\@input, $file);
	close $f;
}

sub frequency
{
	my ($input , $file)= @_;
	my @list =  @{$input};
	$file =~ s/_/ /g;
	$file =~ s/\.txt//g;
	$file =~ s/^.*\///g;
	$times = 0;
	$totalWords = 0;
	foreach $line (@list)
	{
		chomp $line;
		if ($line eq "" or $line =~ /^\s+$/)
		{
			next;
		}
		@newLine = split(/[^a-zA-Z]+/, $line);
		$times += countStr(@newLine);
		$totalWords += removeEmptyStr(@newLine);
	}
	$fre = $times/$totalWords;
	printf ("%4d/%6d = %.9f %s\n", $times, $totalWords, $fre , $file);
}
