#!/usr/bin/perl -w

use Data::Dumper;

my $flag = 0;
if ($ARGV[0] eq "-d")
{
	$flag = 1;
	shift @ARGV;
}

@idFile = @ARGV;

my %hash;
my %total;

getTotalHash();

foreach $f (@idFile)
{
	open(my $F, "<", $f) or die "cannot open < $f: $!";
	chomp(my @id = <$F>);
	close $F;


	my @list;
	foreach $line (@id)
	{
		chomp $line;
		@newLine = split(/[^a-zA-Z]+/, $line);
		push @list, @newLine;
	}

	#my %unique = uniqHash(@list);
	#print Dumper(\%unique);	
	#my %result = getDocFrequency(%unique);

	my %result = getDocFrequency(@list);
	if ($flag == 1)
	{
		for my $name ( sort{ $result{$b} <=> $result{$a}} keys %result) 
		{
    			printf ("%s log_probability of %.1f for %s\n", $f, $result{$name},$name);
		}
	}

	for my $name ( sort{ $result{$b} <=> $result{$a}} keys %result) 
	{
    		printf ("%s most resembles the work of %s (log-probability=%.1f)\n", $f, $name, $result{$name});
    		last;
	}
}

sub getDocFrequency
{
	my %final;

	#print Dumper(\%data);
	#print Dumper(\%total);
	foreach $word (@_)
	{
		$word = lc($word);
		if ($word =~ /^$/ or $word =~/^\s+$/){
			next;
		}
		foreach $file (keys %total) 
		{
			if (exists $total{$file}{$word})
			{
				$result = log(($total{$file}{$word} + 1) / $hash{$file});
				$final{$file} += $result; 
				#* $data{$word};
			} 
			else {
				$result = log(1 / $hash{$file});
				$final{$file} += $result;
			}
		}
	}

	#print Dumper(\%hash);
	#print Dumper(\%final);
	# print keys %data; 
	return %final;
}


sub getTotalHash
{
	foreach $file (glob "lyrics/*txt")
	{
		open (my $f, "<", $file) or die "cannot open < $file: $!";
		chomp(my @input = <$f>);
		close $f;

		my @list;
		my $count = 0;
		foreach $line (@input)
		{
			$line =~ s/[\n\r]+$//;
			$line = lc($line);
			@newLine = split(/[^a-zA-Z]+/, $line);
			#push @list,@newLine;
			foreach(@newLine) 
			{
				if (!($_ =~ /^$/)) {
					$count ++;
					push @list, $_;
				}		
			}

		}
		$file =~ s/_/ /g;
		$file =~ s/\.txt//g;
		$file =~ s/^.*\///g;

		$hash{$file} = $count;
		%newHash = uniqHash(@list);

		foreach $key (keys %newHash)
		{
	    		$total{$file}{$key} = $newHash{$key};
		}
	}
	#print Dumper(\%total);
}

sub uniqHash
{
	my %uniq;
	foreach $word (@_){
		$word = lc($word);
		if  (!$uniq{$word}){
			$uniq{$word} = 1;
		} else {
			$uniq{$word}++;
		}
	}
	return %uniq;
}
