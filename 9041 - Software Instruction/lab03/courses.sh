#!/bin/sh

wget -q -O- "http://www.handbook.unsw.edu.au/vbook2017/brCoursesByAtoZ.jsp?StudyLevel=Undergraduate&descr=All" "http://www.handbook.unsw.edu.au/vbook2017/brCoursesByAtoZ.jsp?StudyLevel=Postgraduate&descr=All" |egrep "$1" | sed 's/<A\ href.*">//g'| sed 's/<TD.*">//g'| sed 's/<.*>//g'| sed 's/^[ \t]*//g'|sed 's/[ \t]*$//g' | grep .|sed "N;s/\n/ /"| egrep '^[A-Z]{4}[0-9]{4}' | sort | uniq
