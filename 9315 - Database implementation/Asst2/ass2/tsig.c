// tsig.c ... functions on Tuple Signatures (tsig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"

// make a tuple signature

// https://www.cse.unsw.edu.au/~cs9315/18s2/lectures/week07/slide005.html

Bits codeword(char *attr_value, int m, int k)
{
   int nbits = 0;
   Bits cword = newBits(m);
   Word random_seed = hash_any(attr_value, strlen(attr_value));
   srandom(random_seed);
   while (nbits < k) {
	   int i = random() % m;
	   if (!bitIsSet(cword, i)){
			 setBit(cword, i);
         	 nbits++;
		}
   }
   return cword;
}

Bits makeTupleSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	Count m = tsigBits(r);
	Count k = codeBits(r);

	Bits tsig = newBits(m);
	Count n = nAttrs(r);
	char ** attrs = tupleVals(r, t);
	int i;
	for(i=0; i<n; i++)
	{
		if (strcmp(attrs[i], "?") == 0) {
			continue;
		}
		// printf("%s %lu\n", attrs[i], strlen(attrs[i]));
		// Bits a_cw = codeword(attrs[i], m, k/n);
		Bits a_cw = codeword(attrs[i], m, k);
		orBits(tsig, a_cw);
	}
	return tsig;
}

// find "matching" pages using tuple signatures

void findPagesUsingTupSigs(Query q)
{
	assert(q != NULL);

	Count m = tsigBits(q->rel);
	Count k = tgSize(q->rel);

	Bits querySig =  makeTupleSig(q->rel, q->qstring);
	// showBits(querySig);
	// putchar('\n');

	File tsigf = tsigFile(q->rel);
	PageID pg_num = nTsigPages(q->rel);
	// printf("pg_num: %d\n", pg_num);

	int p = 1;
	int j;
	for (j=0; j<pg_num; j++)
	{
		Page tpg = getPage(tsigf, j);
		Count maxTup = maxTupsPP(q->rel);
		// printf("Pg_nitems: %d %d\n", pageNitems(tpg), maxTup);
		q->nsigpages++;

		int i;
		for( i=0; i<pageNitems(tpg); i++)
		{
			q->nsigs++;
			Bits btsig = newBits(m);
			setBitsCount(m, btsig);
			setBytesCount(k, btsig);
			getBits(tpg, i, btsig);
			if (isSubset(querySig, btsig)) {
				// showBits(btsig);
				// putchar('\n');
				PageID p_id = iceil(p, maxTup)-1;
				// printf("P_id: %d  %d\n", p_id, p);
				setBit(q->pages, p_id);
			}
			p++;
		}
	}
	// The printf below is primarily for debugging
	// Remove it before submitting this function
	// printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}