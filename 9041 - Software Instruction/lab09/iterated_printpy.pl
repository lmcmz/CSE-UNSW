#!/usr/bin/perl -w

my $line;
my $number = shift(@ARGV);
foreach $var (@ARGV) {
	$line .= $var;
}

sub pl2py {
	$var = $_[0];
	$var =~ s/\\/\\\\/g;
	$var =~ s/\"/\\"/g;
	$var =~ s/\n/\\n/g;
	return "print(\"$var\")";	
}

sub py2pl {
	$var = $_[0];
	$var =~ s/\\/\\\\/g;
	$var =~ s/\"/\\"/g;
	return "print\"$var\"";	
}

$final = $line;

$final = pl2py($final);

if ($number <= 0)
{
	print"$final\n";
	exit;
}

foreach $_ (1..$number)
{	
	$final = pl2py(py2pl($final));
}
print"$final\n";
