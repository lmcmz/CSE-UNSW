#!/usr/bin/perl -w

@Array = <STDIN>;
@orgArray = sort(@Array);
if (@Array ~~ @orgArray){
	print "Fail";
} else {
	print "Pass";
}
