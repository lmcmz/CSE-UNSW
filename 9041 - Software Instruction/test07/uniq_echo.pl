#!/usr/bin/perl -w

my @uniq;
foreach $word (@ARGV){
	if (!($word ~~ @uniq)){
		push(@uniq, $word);
	}
}

print(join(" ", @uniq));
print "\n";
