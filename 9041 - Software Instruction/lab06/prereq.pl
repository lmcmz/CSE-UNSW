#!/usr/bin/perl -w

$course=$ARGV[0];
$url_1 = "http://www.handbook.unsw.edu.au/postgraduate/courses/2018/$course.html";
$url_2 = "http://www.handbook.unsw.edu.au/undergraduate/courses/2018/$course.html";
 
open F, "wget -q -O- $url_2 $url_1|" or die;
while ($line = <F>) {
	if ($line =~ /Prerequisite/){
		$line =~ s/<p>Prerequisite://;
		$line =~ s/<\/p>.*//;
		$line =~ s/^\s+|\s+$//;
		$line =~ s/\.//g;
		$line =~ s/\n//g;
		@array = split / /,$line;
		foreach $string (@array)
		{
			if($string =~ /[A-Z]{4}[0-9]{4}/)
			{
				print "$string\n";
			}
		}
	}
}
