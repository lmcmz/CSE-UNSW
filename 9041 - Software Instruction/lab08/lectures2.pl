#!/usr/bin/perl -w

use Data::Dumper qw(Dumper);

my $dFlag = 0;
my $tFlag = 0;

my %table;

my %weekNumber = (
	"Mon" => "0",
	"Tue" => "1",
	"Wed" => "2",
	"Thu" => "3",
	"Fri" => "4",
);

foreach $course (@ARGV) {
	if ($course eq "-d") {
		$dFlag = 1;
		next;
	}
	
	if ($course eq "-t") {
		$tFlag =1;
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
					if ($dFlag == 1 or $tFlag == 1) {
						changeClass($time, $course, $sem);
					} else {
						print "$course: $sem $time\n";
					}
				}
			} else {
				if ($dFlag == 1 or $tFlag == 1) {
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
					if ($tFlag == 0) {
						print "$sem $course $1 $i\n";	
					}
					
					if (exists $table{$sem}{$weekNumber{$1}}{$i}) {
						$table{$sem}{$weekNumber{$1}}{$i}++;
					} else {
						$table{$sem}{$weekNumber{$1}}{$i} = 1;
					}
					
				}
			} else {
				if ($tFlag == 0) {
					print "$sem $course $1 $i\n";	
				}
				
				if (exists $table{$sem}{$weekNumber{$1}}{$i}) {
					$table{$sem}{$weekNumber{$1}}{$i}++;
				} else {
					$table{$sem}{$weekNumber{$1}}{$i} = 1;
				}
			}
			$hash{$i} = $1;
		}
	} 
}

#print Dumper(\%table);

if ($tFlag == 1) {
	timeTable();	
}

sub timeTable{
	my @week = ("Mon","Tue","Wed","Thu","Fri");
	my @time = ("09:00","10:00","11:00","12:00","13:00","14:00",
				"15:00","16:00","17:00","18:00","19:00","20:00");
	my @timeTable;
	
	foreach $sem (keys %table) {
		unshift @week, $sem;
		print (join("\t", @week));
		print "\n";
		foreach $i (0..11) {
			foreach $j (0..4) {
				if ($j == 0) {
					print "$time[$i]\t";
				}
				if(exists $table{$sem}{$j}{$i+9}){
					print "$table{$sem}{$j}{$i+9}\t";
				} else {
					print "\t";
				}
			}
			print "\n";
		}
		shift @week;	 	
	}
}



