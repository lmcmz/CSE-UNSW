#!/usr/bin/perl

my @all_number;
while($line=<STDIN>)
{
	chmod $line;
	$line =~ s/\s+/ /g;
	$line =~ s/^\s+//g;
	$line =~ s/\s+$//g;
	if ($line eq "")
	{
		next;
	}
	@number = split(" ",$line);
	push @all_number,@number;
	#print("@number\n");
}

my @result;
my $i_pos = 0;
foreach $i (@all_number) {
	$i_pos++;
	my $flag = 1;
	my $j_pos = 0;
	foreach $j (@all_number) {
		$j_pos++;
		if ($i_pos eq $j_pos)
		{
			next;
		}
		#print"$j\n";
		if (int($i) % int($j) eq 0)
		{
			$flag = 0;
			#print "$j,$i\n";
		}
	}
	if ($flag == 1)
	{
		push @result, $i;
	}
}
@result = sort {$a <=> $b} @result;
print "@result\n";

