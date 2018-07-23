#!/usr/bin/perl -w 


my @allCourse;

sub findCourse
{
	$course = $_[0];
	$url_1 = "http://www.handbook.unsw.edu.au/postgraduate/courses/2018/$course.html";
	$url_2 = "http://www.handbook.unsw.edu.au/undergraduate/courses/2018/$course.html";

	my @courses;

	open F, "wget -q -O- $url_2 $url_1|" or die;
	while ($line = <F>) 
	{
        	if ($line =~ /Prerequisite/)
		{
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
					$string =~ s/\,//g;
					$string =~ s/\]//g;
					$string =~ s/\[//g;
					$string =~ s/\(|\)//g;	
					push @courses,$string;
					push @allCourse, "$string\n";			
				}																					
	   		}
		}																							
	}
	return @courses;
}

sub test
{
	foreach $f (@_)
	{
		test(findCourse($f));
	}
}

if ($ARGV[0] eq "-r")
{
	$course=$ARGV[1];
	test($course);
}else {
	$course=$ARGV[0];
	findCourse($course)
}


my @unique = do { my %seen; grep { !$seen{$_}++ } @allCourse };
@unique = sort(@unique);
print @unique;

