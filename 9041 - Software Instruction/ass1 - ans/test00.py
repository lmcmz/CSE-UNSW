#!/usr/bin/python3

foreach $letter ("Runoob") {
	if($letter eq  'b') {
		last;
	}
	print "'当前字母为 :', $letter\n";
  
}
$var = 10;                   
while($var  >  0) {
     print "'当期变量值为 :', $var\n";
	$var = $var -1;
	if($var ==  5) {
		last;
		}
}
print "Good bye!\n";