#!/usr/bin/perl -w

foreach $course (@ARGV) {
	$url = " http://timetable.unsw.edu.au/current/${course}.html";
	open F, "wget -q -O- $url |" or die;

	my @array;
	my $count = 0;
	my @total;
	while ($line = <F>) {
		if ($line =~ /[Ll]ecture/) {
			$line =~ /(#S\d{1}-\d{4})/;
		if ($1) { push @array, $count;}
	}
	$count++;
	push(@total, $line)
	}

	my %hash;
	foreach $number (@array) {
		$sem = $total[$number + 1];
		$sem =~ m/(T\d{1})/;
		$sem = $1;
		$sem =~ s/T/S/;
		$time = $total[$number + 6];
		$time =~ m/>(.*)</;
		$time = $1;
		if ($time !~ /^$/) {
			if(exists $hash{$time})
			{
				if($hash{$time} ne $sem){
					print "$course: $sem $time\n";
				}
			} else {
				print "$course: $sem $time\n";
			}	
			$hash{$time} = $sem;
		}
	}
} 

