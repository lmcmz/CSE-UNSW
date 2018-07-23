#!/usr/bin/perl -w

$flag = 0;

foreach $course (@ARGV) {
	if ($course eq "-d") {
		$flag = 1;
		next;
	}
	
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
					if ($flag == 1) {
						changeClass($time, $course, $sem);
					} else {
						print "$course: $sem $time\n";
					}
				}
			} else {
				if ($flag == 1) {
					changeClass($time, $course, $sem);
				} else {
					print "$course: $sem $time\n";
				}
			}
			$hash{$time} = $sem;
		}
	}		
} 

sub changeClass{
	my %hash;
	my($time , $course, $sem) = @_;
	$time =~ s/\([^\)]*\)//g;
	$time =~ s/:\d0//g;
	@array = split(',' , $time);
	foreach $string (@array) {
		$string =~ /(\d{2}) - (\d{2})/;
		$hour = ($2 - $1) - 1;
		foreach $i ($1..($1+$hour)) {
			$string =~ /([a-zA-Z]{3})/;
			
			if(exists $hash{$i})
			{
				if($hash{$i} ne $1){
					print "$sem $course $1 $i\n";
				}
			} else {
				print "$sem $course $1 $i\n";
			}
			$hash{$i} = $1;
		}
	} 
}



