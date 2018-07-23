#!/usr/bin/perl -w

my $flag = 0;
if ($ARGV[0] eq "-d")
{
	$flag = 1;
	shift @ARGV;
}

@idFile = @ARGV;

my %hash;

sub countStr
{
	my ($list, $word) = @_;
	@myList = @{$list};
	$count = 0;
	foreach $str (@myList)
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

sub frequency
{
	my ($input , $file, $word)= @_;
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
		$times += countStr(\@newLine, $word);
		$totalWords = $hash{$file};
	}
	$fre = log(($times+1)/$totalWords);
	return $fre;
	#printf ("log((%d+1)/%6d) = %8.4f %s\n", $times, $totalWords, $fre , $file);
}

sub find
{
	my %data = @_;
	my %final;
	my @list = keys %data;

	foreach $word (@list)
	{
		my $result = 0;
		$word = lc($word);
		foreach $file (glob "lyrics/*.txt") 
		{
			my $result = 0;
			my $name = $file;
			$name =~ s/_/ /g;
			$name =~ s/\.txt//g;
			$name =~ s/^.*\///g;
			open(my $f, "<", $file) or die "cannot open < input.txt: $!";
			chomp(my @input = <$f>);
			$result += frequency(\@input, $file, $word);
			$final{$name} += $result * $data{$word};
			close $f;
		}
	}
	#print "@{[%final]}\n";
	return %final;
}


sub total{
	foreach $file (glob "lyrics/*txt"){
	open (my $f, "<", $file) or die "cannot open < $file: $!";
	chomp(my @input = <$f>);
	close $f;
	$count = 0;
	foreach $line (@input){
		@words = split /[^A-Za-z]/, $line;
		foreach(@words) {
		if (!($_ =~ /^$/)) {
			$count ++;
			}		
		}
	    }
		$file =~ s/_/ /g;
		$file =~ s/\.txt//g;
		$file =~ s/^.*\///g;
	    	$hash{$file} = $count;
	}
}

sub uniqHash
{
	my %uniq;
	foreach $word (@_){
		$word = lc($word);
		if  (!$uniq{$word}){
			$uniq{$word} = 1;
		} else {
			$uniq{$word} ++;
		}
	}
	return %uniq;
}

total();

foreach $f (@idFile)
{
	open(my $F, "<", $f) or die "cannot open < input.txt: $!";
	chomp(my @id = <$F>);
	close $F;

	my @list;
	foreach $line (@id)
	{
		chomp $line;
		@newLine = split(/[^a-zA-Z]+/, $line);
		push @list, @newLine;
	}

	my %unique = uniqHash(@list);
	my %result = find(%unique);

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

