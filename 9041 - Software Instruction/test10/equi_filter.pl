#!/usr/bin/perl -w

@input = <STDIN>;

foreach $line (@input) {
	@array = split(' ',$line);
	my $line_count = -1;
	foreach $word (@array) {
		$line_count++;
		@chars = split('',$word);
		my $number = 0;
		my $count = -1;
		foreach $letter (@chars) {
			$count++;
			if ($#chars == 0) {
				if ($line_count == $#array) {
					print "$word";
				} else {
					print "$word ";	
				}
				#print "$word ";
				last;
			}
			if ($number == 0) {
				$number = findFrequency(\@chars,$letter);
			}
			if ($count == $#chars and $number == findFrequency(\@chars,$letter)) {
				if ($line_count == $#array) {
					print "$word";
				} else {
					print "$word ";	
				}
			}
			if ($number != findFrequency(\@chars,$letter)) {
				last;
			}
		}
	}
	print "\n";
}

sub findFrequency {
	my ($array_ref, $char) = @_;
	my @array = @{$array_ref};
	my $count = 0;
	#print "@array\n";
	foreach $letter (@array) {
		if (lc($letter) eq lc($char)) {
			$count++;
		}
	}
	return $count;
}
