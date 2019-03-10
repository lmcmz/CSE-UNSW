// bsig.c ... functions on Tuple Signatures (bsig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"
#include "psig.h"
#include "bits.h"

void findPagesUsingBitSlices(Query q)
{
	assert(q != NULL);

	File bsigf = bsigFile(q->rel);
	Count pm = psigBits(q->rel);
	Count bm = bsigBits(q->rel);

	Count bsig_size = bgSize(q->rel);
	Count one_pg_bytes = PAGESIZE - sizeof(int);
	Count one_pg_num = one_pg_bytes/bsig_size;

	Bits qsig = makePageSig(q->rel, q->qstring);
	// showBits(qsig);
	// putchar('\n');
	
	Count val_nB = nBsigs(q->rel);

	setAllBits(q->pages);

	int i;
	for(i = 0; i<pm-1; i++)
	{
		if (!bitIsSet(qsig, i)) {
			continue;
		}

		PageID b_pid = iceil(i+1, one_pg_num) - 1;
		Page b_pg = getPage(bsigf, b_pid);
		Bits b_bits = newBits(one_pg_bytes * 8);
		getBits(b_pg, 0, b_bits);

		Count offset = bm * (i % one_pg_num);

		int j;
		for (j=0; j<val_nB; j++) 
		{
			if (!bitIsSet(q->pages, j)) {
				continue;
			}
			if (!bitIsSet(b_bits, offset+j)) {
				// printf("NO MATHCH: %d\n", j);
				unsetBit(q->pages, j);
			}
		}
	}

	q->nsigpages = iceil(pm, one_pg_num);
	q->nsigs = val_nB;
}

