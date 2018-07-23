#!/usr/bin/perl -w

# Starting point for COMP[29]041 assignment 1
# http://www.cse.unsw.edu.au/~cs2041/assignments/pypl
# written by z5102511@unsw.edu.au Oct 2017

#no warnings 'experimental::smartmatch';

my @varList;
my @stackList;

while ($line = <>) {
    print(translate($line))     #Translate the script
}

if ($#stackList != -1){        #if the stackList is not empty, print all {
    foreach $_ (@stackList) {
        print"}\n";
    }
}

sub translate                            #Tanslate    script
{
    $line = $_[0];
    if ($line =~ /^#!/ && $. == 1) {     #Convert  the first    line

        return "#!/usr/bin/perl -w\n";

    } elsif ($line =~ /^\s*(#|$)/) {      #Ingnore Commments
        
        return $line;

    } elsif ($line =~ /^import/) {         #Ingnore import
            
        return "\n";

    } elsif ($line =~ /^(\s*)break/) {      #Break  <==> last
        my $tab = $1;
        my $flag = "";
        if (checkTab($tab)) {
           # print"1\n";
            $flag = "${tab}\}\n";
        }
        return "${flag}${tab}last;\n";

    } elsif ($line =~ /^(\s*)sys.stdout.write\(\"(.*)\"\)/) {      #sys.stdout.write  <==> print
        my $tab = $1;
        my $content = $2;
        my $flag = "";
        if (checkTab($tab)) {
            #print"1\n";
            $flag = "${tab}\}\n";
        }
        return "${flag}${tab}print \"$content\";\n";

    } elsif ($line =~ /^(\s*)(.*)\s*=.*sys.stdin.readlines()/) { #sys.stdin.readlines() == <STDIN>
        my $tab = $1;
        my $var = $2;
        $var =~ s/^\s+|\s+$//g;
        addVar($var);
        $flag = "";
        if (checkTab($tab)) {
            #print"1\n";
            $flag = "${tab}\}\n";
        }
        return "${flag}${tab}\@$var = <STDIN>;\n";

    } elsif ($line =~ /^(\s*)(.*)\s*=.*sys.stdin$/) {        #sys.stdin <==> <STDIN>
        my $tab = $1;
        my $var = $2;
        $var =~ s/^\s+|\s+$//g;
        addVar($var);
        $flag = "";
        if (checkTab($tab)) {
            #print"1\n";
            $flag = "${tab}\}\n";
        }
        return "${flag}${tab}\$$var = <STDIN>;\n";

    } elsif ($line =~ /^(\s*)print\("(.*)"\)$/) {        #print() <==> print ""
        my $tab = $1;
        my $flag = "";
        if (checkTab($tab)) {
            #print"2\n";
            $flag = "${tab}\}\n";
        }
        
        return "${flag}${tab}print \"$2\\n\";\n";

    } elsif ($line =~ /^(\s*)for\s*(\w*)\s*in\s*(.*):/) {        #for ... in <==> foreach ()
        my $tab = $1;
        my $var = $2;
        my $range = $3;
        my $flag = "";
        if (checkTab($tab)) {
            $flag = "${tab}\}\n";
        }
        countTab($tab);
        $var =~ s/^\s+|\s+$//g;
        addVar($var);
        my $newRange = $range;
        if ($range =~ /range\((.*)\)/) {
            $number = $1;
            $number =~ s/^(.*), (.*)//g;
            $new1 = testVar($1);
            $new2 = testVar($2);
            $new2 = rangeIssue($new2);
            $newRange = "$new1\.\.$new2";
        }
        if ($range =~ /sys.stdin/) {
            $newRange = "<STDIN>";
        }
        return "${flag}${tab}foreach \$$var ($newRange) {\n";
        
    } elsif ($line =~ /^(\s*)if(.*):(.*)/) {        #if
        my $tab = $1;
        my $condition = $2;
        my $test = $3;
        my $flag = "";
        if (checkTab($tab)) {
            $flag = "${tab}\}\n";
        }
        countTab($tab);
        #$condition = testVar($condition);
        $condition =~ s/^\s*(.*)([<=>]=?)(.*)/\$$1 $2 $3/g;        #Convert condition
        $condition =~ s/([<>=])\s+=/$1=/g;
        $test = translate($test);            #Convert the lines
        return "${flag}${tab}if($condition) {\n$test";

    } elsif ($line =~ /^(\s*)elif\s*(.*)\s*(.*)\s*(.*):/) {        #elif
        my $tab = $1;
        my $var = $2;
        my $symbol = $3;
        my $var2 = $4;
        my $flag = "";
        if (checkTab($tab)) {                #print }
            $flag = "}";
        }
        countTab($tab);
        if ($var2 ~~ @varList) {
            $var2 = "\$$var2";
        }
        return "${tab}${flag}elsif (\$$var $symbol $var2){\n";

    } elsif ($line =~ /^(\s*)else:/) {        #else
        my $tab = $1;
        my $flag = "";
        if (checkTab($tab)) {
            $flag = "}";
        }
        countTab($tab);
        return "${tab}${flag}else{\n";

    } elsif ($line =~ /^(\s*)while(.*):(.*)/) {       #While
        my $tab = $1;
        my $condition = $2;
        my $test = $3;
        $condition =~ s/^\s*(.*)([<>=]=?)(.*)/\$$1 $2 $3/g;    #Convert condition
        $secondVar = $3;
        $secondVar =~ s/^\s*|\s*$//g;        #Trim the var
        if ($secondVar ~~ @varList) {
            $condition =~ s/$secondVar/\$$secondVar/g;
        }
        $condition =~ s/([<>=])\s+=/$1=/g;
        my @array;
        my @array2;
        $flag = "";
        if (checkTab($tab)) {            #check should print } or not
            $flag = "${tab}\}\n";
        }
        countTab($tab);                #Add tab
        if ($test =~ /;/) {
            $test =~ s/;/;\n/g;
            @array = split(';',$test);
        } else {
            push(@array, $test);
        }
        
        foreach $test2 (@array) {
            push(@array2,translate($test2));
        }
        $newLine = join('', @array2);        #Add $ before var
        return "${flag}${tab}while($condition) {\n$newLine";

    }elsif ($line =~ /^(\s*)(.*) = (.*)/) {           #Deal with (.*) = (.*)
        my $tab = $1;
        my $var = $2;
        my $newLine = $3;
        $flag = "";
        if (checkTab($tab)) {
            $flag = "${tab}\}\n";
        }
        $var =~ s/^\s+|\s+$//g;
        addVar($var);                            #add var in to stackList
        $newLine = intDivi($newLine);            # //  --->  /
        $newLine = testVar($newLine);            #add $ before var
        $newLine = replaceLen($newLine);         #len() ---> $#   
        $newLine = replaceInput($newLine);       #sys.stdin.readline  <==>   <STDIN>
        
        if($newLine eq isArray($newLine)){        #Is a array or not
            $var =~ s/^/\$/g;
        } else {
            $newLine = isArray($newLine);
            $var =~ s/^/\@/g;
        }
        return "${flag}${tab}$var = $newLine\;\n";

    } elsif ($line =~ /^(\s*)print\s*\((.*)\)$/) {        #Print
        my $tab = $1;
        my $newLine = $2;
        my $flag = "";
        if (checkTab($tab)) {
            $flag = "${tab}\}\n";
        }
        #print "$newLine\n";
        #print "@varList\n";
        if ($newLine =~ /\"(.*)\" % (.*)/) {                #If it contain "%"
                $newLine = printFormat($newLine);
                return "${flag}${tab}print \"$newLine\";\n"; 
        }
        $newLine = testVar($newLine);                       #Add $ before var
        if ($newLine =~ /end=/) {                           #Deal with "end=' '"
            $newLine = replaceEnd($newLine);
            return "${flag}${tab}print \"$newLine\";\n"; 
        }
        
        if ($newLine =~ m/\*/) {
            return "${tab}print $newLine,\"\\n\";\n";
            next;
        }
        
        return "${flag}${tab}print \"$newLine\\n\";\n";

    } elsif ($line =~ /^(\s*)(.*)\.append\((.*)\)/) {        #Deal with apppend
        my $tab = $1;
        my $list = $2;
        my $var = $3;
        $flag = "";
        if (checkTab($tab)) {
            $flag = "${tab}\}\n";
        }
        return "${flag}${tab}push \@$list, \$$var;\n";

    } else {
        return "#$line\n";
    }
}

sub printFormat        #convert print(" "% )  to print "  "
{
    my $var = $_[0];
    $var =~ /\"(.*)\" % (.*)/;
    my $pre = $1;
    my $end = $2;
    $end = testVar($end);
    $pre =~ s/\%\w+/$end/g;
    return $pre;
}

sub replaceEnd        # end='  '  <==>   '  '
{
    my $var = $_[0];
    $var =~ s/,\s*end\s*=\s*\'(.*)\'/$1/g;
    return $var;
}


sub replaceLen           #Length of the array len()  <==>   $#
{
    my $var = $_[0];
    $var =~ s/len\(\$(.*)\)/\$\#$1 + 1/g;
    return $var;
}

sub replaceInput         #sys.stdin.readline  <==>   <STDIN>
{
    my $var = $_[0];
    $var =~ s/sys.stdin.readline\(\)/<STDIN>/g;
    return $var;
}

sub addVar            #Add var in array
{
    my $var = $_[0];
    $var =~ s/^\s+|\s+$//g;
    if (!($var ~~ @varList)){        #If array contain it before, don't add it 
        push(@varList, $var);
    }
}

sub testVar            #Check it is a var or not
{
    my $newLine = $_[0];
    foreach $var (@varList) {
        if ($newLine =~ m/\W*($var)\W*/) {                          #Check is there a var or not
            $newLine =~ s/(\W*)($var)(\W*)/${1}\$${2}${3}/g;        #Add $ before var
        }
    }
    $newLine =~ s/(\w+)\$(\w+)/${1}${2}/g;
    $newLine =~ s/\$\$/\$/g;
    return "$newLine";
}

sub isArray            #List convert to array
{
    my $var = $_[0];
    if ($var =~ m/\[(.*)\]/) {
        return "qw($1)";
    }
    return $var;
}

sub intDivi                #Covert '//'  >  '/' 
{
    my $newLine = $_[0];
    if ($newLine =~ m/\s*(.*)\s*\/\/\s*(.*)\s*/) {
        $newLine = "$1 / $2";
    }
    return "$newLine";
}
    
sub rangeIssue                        #Covert range function 
{
    my $var = $_[0];
    $var =~ s/^\s*|\s*$//g;
    if ($var =~ m/^(\d+)$/) {
        $number = $1 - 1;
        return "$number";
    } elsif ($var =~ m/^(.*)\s*\+\s*(\d+)$/) {
        $number = $2 - 1;
        return "$1 + $number";
    }
}

sub countTab                                #Add number of tab in to stack
{
    my $line = $_[0];
    my $count = length($line);
    if (!($count ~~ @stackList)){
        push(@stackList, $count);
    }
    #push @stackList,$count;
}

sub checkTab                                #Remove number of tab out of stack, and print {
{
    my $line = $_[0];
    my $count = length($line);
    
    if($#stackList == -1) {
        return 0;
    }

    if ($count eq $stackList[$#stackList]){
        pop(@stackList);
        return 1;
    } elsif ($count ~~ @stackList) {
        my ($index) = grep { $stackList[$_] ~~ $count } 0 .. $#stackList;
        my $flag = "";
        foreach $_ (0..$#stackList-$index) {
            my $j = pop(@stackList);
            foreach (0..$j) {
                $flag .= "\t";
            }
            $flag .= "}\n";
        }
        print $flag;
        return 0;
    } else {
        return 0;
    }
}
