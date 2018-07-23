#!/usr/bin/perl -w

$def_line=10;


if ($#ARGV == -1) {
	@string=<STDIN>;
	$test=$#string - $def_line + 1;
	if ($test < 0) {
	    $test = 0;
	}
	my @test = @string[$test..$#string];
	print @test;
	exit 0;
}

foreach $arg (@ARGV) {
    if ($arg eq "--version") {
        print "$0: version 0.1\n";
        exit 0;
    }

    if ($arg =~ /^-[0-9]+$/){
    	$arg =~ s/^-//;
    	$def_line = $arg;
    } else{
        push @files, $arg;
    }
}


foreach $f (@files) {
    if ($#files > 1) {
    	print "==> $f <==\n";
    }
    open(my $F, '<', $f) or die "$0: Can't open $f: $!\n";
    my @Array;
    while(<$F>){
    	push @Array, $_;
    }

    $test=$#Array - $def_line + 1;
    if ($test < 0) {
    	$test = 0;
    }
    my @test = @Array[$test..$#Array];
    print @test;
    close $F;
}

