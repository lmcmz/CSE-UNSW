#!/bin/bash

#set -x

mkdir "$1"
cp "$2" "$1"

Source="`wget -q -O- 'https://en.wikipedia.org/wiki/Triple_J_Hottest_100?action=raw'| egrep '\[\[Triple\ J\ Hottest\ 100|^#'|sed 's/^|style.*\[T/T/g'|sed 's/\[\[//g'|sed 's/\]\]//g'|sed 's/|[0-9].*$//g'|sed 's/(.*song).*"/"/g'| sed 's/^.*|.*Triple/Triple/g'| sed 's/^#\ /#/g'|tr '#' '\t'|sed 's/^Triple/\nTriple/g'|egrep -v "All\ [Tt]ime|the\ Past"| egrep -v '\([0-9]*\)$'| egrep .|sed 's/^Triple/\nTriple/g'`"

Temp="`wget -q -O- 'https://en.wikipedia.org/wiki/Triple_J_Hottest_100?action=raw'| egrep '\[\[Triple\ J\ Hottest\ 100|^#'|sed 's/^|style.*\[T/T/g'|sed 's/\[\[//g'|sed 's/\]\]//g'|sed 's/|[0-9]\{4\}.*$//g'| sed 's/^#\ /#/g'|tr '#' '\t'|sed 's/^|\ style.*Tri/Tri/g'|egrep -v "All\ [Tt]ime|the\ Past"| egrep -v '\([0-9]*\)$'|sed 's/^Triple/\nTriple/g'|sed 's/([^)]*[^As])//g'| sed 's/[ \t]*$//g' |sed 's/|[a-zA-Z\ ]*"$//g'| sed 's/^[ \t]*//g'|sed 's/^[a-zA-Z]*\ |//g'|sed 's/"//g'|sed 's/\xE2\x80\x93/%/g'|sed 's/|.*\ %/\ %/g'|sed 's/\(.*\)%\(.*\)/\2\ %\ \1/g'|tr '/' '%' |sed 's/\ *%\ */\ %\ /g'| tr '%' '-'| sed 's/^[ \t]*//g'|egrep .`"

Test="`wget -q -O- 'https://en.wikipedia.org/wiki/Triple_J_Hottest_100?action=raw'| egrep '\[\[Triple\ J\ Hottest\ 100|^#'|sed 's/^|style.*\[T/T/g'|sed 's/\[\[//g'|sed 's/\]\]//g'|sed 's/|[0-9]\{4\}.*$//g'| sed 's/^#\ /#/g'|tr '#' '\t'|sed 's/^|\ style.*Tri/Tri/g'|egrep -v "All\ [Tt]ime|the\ Past"| egrep -v '\([0-9]*\)$'|sed 's/^Triple/\nTriple/g'|sed 's/([^)]*[^As])//g'| sed 's/[ \t]*$//g' |sed 's/|[a-zA-Z\ ]*"$//g'| sed 's/^[ \t]*//g'|sed 's/^[a-zA-Z]*\ |//g'|sed 's/"//g'|sed 's/\xE2\x80\x93/%/g'|sed 's/|.*\ %/\ %/g'|sed 's/\(.*\)%\(.*\)/\2\ %\ \1/g'|sed 's/\//\ -\ /g' |sed 's/\ *%\ */\ %\ /g'| tr '%' '-'| sed 's/^[ \t]*//g'|egrep .|sed 's/^.*\ |//g'|sed 's/Korn/Ko\xD0\xAFn/g'|
#sed "s/"9 - Ol' Man River - TISM"/"9 - (He'll Never Be An) Ol' Man River - TISM"/g"|
sed 's/"1 - Pretty Fly - The Offspring"/"1 - Pretty Fly (for a White Guy) - The Offspring"/g'|
sed 's/"10 - Short Skirt-Long Jacket - Cake"/"10 - Short Skirt - Long Jacket - Cake"/g'|
#sed 's/"9 - Clint Eastwood - Gorillaz featuring Del the Funky Homosapien"/"9 - Clint Eastwood (song) - Gorillaz featuring Del tha Funkee Homosapien"/g'|
#sed "s/"4 - On My Mind - Powderfinger"/"4 - (Baby I've Got You) On My Mind - Powderfinger"/g"|
sed 's/"5 - Clocks - Coldplay"/"5 - Clocks (Röyksopp Remix) - Coldplay"/g'|
sed 's/"10 - Jolene - The White Stripes"/"10 - Jolene (live) - The White Stripes"/g'|
sed 's/"5 - Dare - Gorillaz featuring Shaun Ryder"/"5 - DARE - Gorillaz featuring Shaun Ryder"/g'|
sed 's/"10 - 19-20-20 - The Grates"/"10 - 19 - 20 - 20 - The Grates"/g'|
sed 's/"7 - Harder, Better, Faster, Stronger - Daft Punk"/"7 - Harder, Better, Faster, Stronger (Alive 2007) - Daft Punk"/g'|
sed 's/"7 - The Festival Song - Pez Featuring 360"/"7 - The Festival Song - Pez Featuring 360 and Hailey Cramer"/g'|
sed 's/"10 - Somebody to Love Me - Mark Ronson"/"10 - Somebody to Love Me - Mark Ronson & The Business Intl. featuring Boy George and Andrew Wyatt"/g'|
sed 's/"7 - Fuck You - Cee-Lo Green"/"7 - Fuck You - Cee - Lo Green"/g'|
sed 's/"10 - I Love It - Hilltop Hoods Featuring Sia"/"10 - I Love It - Hilltop Hoods Featuring Sia Furler"/g'|
sed 's/"8 - Boys like You - 360"/"8 - Boys Like You - 360 featuring Gossling"/g'|
sed 's/"3 - Breezeblocks - Alt-J"/"3 - Breezeblocks - Alt - J"/g'|
sed 's/"3 - Get Lucky - Daft Punk featuring Pharrell"/"3 - Get Lucky - Daft Punk featuring Pharrell Williams"/g'|
sed 's/"10 - King and Cross - \xC3\x81sgeir"/"10 - King and Cross - \xC3\x81sgeir Trausti"/g'|
sed 's/"9 - Chandelier - Sia"/"9 - Chandelier - Sia Furler"/g'|
sed 's/"4 - 1955 - Hilltop Hoods featuring Montaigne"/"4 - 1955 - Hilltop Hoods featuring Montaigne and Tom Thum"/g'|
sed 's/"5 - Redbone - Donald Glover"/"5 - Redbone - Childish Gambino"/g'`"
#sed 's/"6 - Believe - DMA's"/"6 - Believe (Like a Version) - DMA's"/g'
#`"

Albums="`echo "$Source" | egrep '^Triple'`"

cd "$1"
echo "$Albums" | while read album
do
        mkdir "$album"
done

cp "$2" "`echo "$Albums"|head -n1`"
cd "`echo "$Albums"|head -n1`"


i=0
echo "$Test"|while read song
do
	if echo "$song" | fgrep -vc "Triple">/dev/null
	then
		i=`expr $i + 1`
		name=`echo "$i - ""$song"".mp3"`
		cp "$2" "$name"
	else	
		cd ../
		cp "$2" "$song"
		cd "$song"
		i=`expr 0`>/dev/null
	fi	
done

#find . -name "$2" | xargs rm -f
cd ../
rm -r "$2"
rm -r ./*/"$2"
