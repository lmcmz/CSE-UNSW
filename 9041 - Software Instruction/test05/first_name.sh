#!/bin/sh

egrep 'COMP[29]041' "$1"| cut -f3 -d'|'|uniq|cut -f2 -d','| sed 's/^\ *//'|sed 's/\ *$//'|sort|cut -f1 -d' '| uniq -c|sort -nr| head -1|sed 's/^[0-9\ ]*//g'
