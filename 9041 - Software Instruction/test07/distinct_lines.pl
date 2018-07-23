#!/usr/bin/perl -w

my $number = $ARGV[0];
my @uniq = [];
my $count = 0;

while ($line=<STDIN>){

	$count++;
	chomp($line);
	$line = lc($line);
	$line =~ s/\s+/ /g;
	$line =~ s/^\s+|\s+$//g;
	#print "$line\n";
	if (!($line ~~ @uniq)){
		push @uniq, $line;
	}

	if($#uniq == $number ){
		#$count -= 1; 
		print "$#uniq distinct lines seen after $count lines read.\n";
		last;
	}

}

if ($#uniq < $number)
{
#	print "@uniq \n";
	print "End of input reached after $count lines read - $number different lines not seen.\n"
}

