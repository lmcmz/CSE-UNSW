// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "hash.h"
#include "tsig.h"

Bits makePageSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	Count pm = psigBits(r);
	Count k = codeBits(r);

	Bits psig = newBits(pm);
	Count n = nAttrs(r);
	char ** attrs = tupleVals(r, t);
	int i;
	for(i=0; i<n; i++)
	{
		// printf("%s %lu\n", attrs[i], strlen(attrs[i]));
		if (strcmp(attrs[i], "?") == 0) {
			continue;
		}
		Bits a_cw = codeword(attrs[i], pm, k);
		orBits(psig, a_cw);
	}
	return psig;
}

void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
	
	Count pm = psigBits(q->rel);
	Count pk = pgSize(q->rel);

	Bits querySig = makePageSig(q->rel, q->qstring);
	// showBits(querySig);
	// putchar('\n');

	File psigf = psigFile(q->rel);
	PageID pg_num = nPsigPages(q->rel);

	int p = 0;
	int j;

	for (j=0; j<pg_num; j++)
	{
		Page ppg = getPage(psigf, j);
		// Count maxTup = maxTupsPP(q->rel);
		q->nsigpages++;

		// TODO  strange solution
		Count items = pageNitems(ppg);
		if (j == pg_num - 1) {
			items = pageNitems(ppg)+1;
		}
		// printf("pg_num: %d - %d\n", j, pageNitems(ppg));

		int i;
		for(i=0; i<items; i++)
		{
			q->nsigs++;
			Bits btsig = newBits(pm);
			setBitsCount(pm, btsig);
			setBytesCount(pk, btsig);
			getBits(ppg, i, btsig);
			if (isSubset(querySig, btsig)) {
				// printf("Match: %d %d\n", p, q->nsigs);
				setBit(q->pages, p);
			} else {
				// printf("No Match: %d\n", p);
			};
			p++;
		}
	}
}

