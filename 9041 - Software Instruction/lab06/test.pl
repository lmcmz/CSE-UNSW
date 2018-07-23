#!/usr/bin/perl


foreach $n (1..10) {
    last if ($n > 5);
        print "$n ";
	    next if ($n % 2 == 0);
	        print "$n ";
		}
		print "\n";
