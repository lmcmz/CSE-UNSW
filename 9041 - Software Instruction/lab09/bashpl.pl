#!/usr/bin/perl -w

$file = $ARGV[0];

open(my $F, "<", "$file") or die "cannot open < $file: $!";

my @varList;

my @keyList = ("while","{","}");

while ($line = <$F>)
{
	chomp($line);
	if ($line eq "#!/bin/bash") {
		$line = "#!/usr/bin/perl -w";
	}
	$line =~ s/=/ = /g;
	$line =~ s/do$/{/g;
	$line =~ s/done$/}/g;
	$line =~ s/then$/{/g;
	$line =~ s/fi$/}/g;
	$line =~ s/\)\)/)/g;
	$line =~ s/\(\(/(/g;
	$line =~ s/echo/print "/g;
	if ($line =~ /print/) {
		$line =~ s/$/\\n"/g;
	}
	if ($line =~ m/^(\w*) =/ or $line =~ m/^\s+(\w*) = \$/) {
		if (!($1 ~~ @varList)){
			push(@varList, $1);	
		}
	}
	
	foreach $var (@varList) {
		if ($line =~ /print/) {
			last;
		}
		if ($line =~ m/$var/) {
			$line =~ s/^$var/\$$var/g;
			$line =~ s/ $var\W/\$$var/g;
			$line =~ s/\($var/\$$var/g;
			if ($line =~ m/^\s+($var)/) {
				$line =~ s/$1/\$$var/g;
			}
		}
	}
	$line =~ s/\$\$/\$/g;
	$line =~ s/\)//g;
	$line =~ s/ *= */ = /g;
	$line =~ s/< *=/<=/g;
	$line =~ s/> *=/>=/g;
	$line =~ s/= *=/==/g;
	$line =~ s/! *=/!=/g;
	
	if ($line =~ /while/) {
		$line =~ s/while/while (/g;
		$line =~ s/\( *\(/(/g;
		$line =~ s/$/)/g;
	}
	if ($line =~ /if/) {
		$line =~ s/if/if (/g;
		$line =~ s/\( *\(/(/g;
		$line =~ s/$/)/g;
	}
	$line =~ s/else/}else{/g;
	
	$line =~ s/\$\(/\$/g;
	$line =~ s/\$\$/\$/g;
	if ($line =~ /\$\d/) {
		$line =~ s/(\$)(\d)/$2/g;
	}
	
	if ($line =~ /^[^#]*$/ and $line !~ /^$/ and !($line ~~ @keyList) and $line !~ /while/ and  $line !~ /\s*{|}|do|done|if|else/) {
		$line =~ s/$/;/g;
	}

	print "$line\n";
}
