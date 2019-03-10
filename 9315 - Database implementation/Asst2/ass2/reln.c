// reln.c ... functions on Relations
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "defs.h"
#include "reln.h"
#include "page.h"
#include "tuple.h"
#include "tsig.h"
#include "bits.h"
#include "hash.h"

#include "psig.h"
// open a file with a specified suffix
// - always open for both reading and writing

File openFile(char *name, char *suffix)
{
	char fname[MAXFILENAME];
	sprintf(fname,"%s.%s",name,suffix);
	File f = open(fname,O_RDWR|O_CREAT,0644);
	assert(f >= 0);
	return f;
}

// create a new relation (five files)
// data file has one empty data page

Status newRelation(char *name, Count nattrs, float pF,
                   Count tk, Count tm, Count pm, Count bm)
{
	Reln r = malloc(sizeof(RelnRep));
	RelnParams *p = &(r->params);
	assert(r != NULL);
	p->nattrs = nattrs;
	p->pF = pF,
	p->tupsize = 28 + 7*(nattrs-2);
	p->tupPP = (PAGESIZE-sizeof(Count))/p->tupsize;
	p->tk = tk; 
	if (tm%8 > 0) tm += 8-(tm%8); // round up to byte size
	p->tm = tm; p->tsigSize = tm/8; p->tsigPP = PAGESIZE/(tm/8);
	if (pm%8 > 0) pm += 8-(pm%8); // round up to byte size
	p->pm = pm; p->psigSize = pm/8; p->psigPP = PAGESIZE/(pm/8);
	if (p->psigPP < 2) { free(r); return -1; }
	if (bm%8 > 0) bm += 8-(bm%8); // round up to byte size
	p->bm = bm; p->bsigSize = bm/8; p->bsigPP = PAGESIZE/(bm/8);
	if (p->bsigPP < 2) { free(r); return -1; }
	r->infof = openFile(name,"info");
	r->dataf = openFile(name,"data");
	r->tsigf = openFile(name,"tsig");
	r->psigf = openFile(name,"psig");
	r->bsigf = openFile(name,"bsig");
	addPage(r->dataf); p->npages = 1; p->ntups = 0;
	addPage(r->tsigf); p->tsigNpages = 1; p->ntsigs = 0;
	addPage(r->psigf); p->psigNpages = 1; p->npsigs = 0;
	// addPage(r->bsigf); p->bsigNpages = 1; p->nbsigs = 0; // replace this
	// Create a file containing "pm" all-zeroes bit-strings,
    // each of which has length "bm" bits
	Count one_pg_bytes = PAGESIZE - sizeof(int);
	Count one_pg_num = one_pg_bytes/p->bsigSize;
	Count b_pgnum = iceil(p->pm, one_pg_num); 
	p->bsigNpages = 0;
	// printf("b_pgnum: %d\n", b_pgnum);
	int i;
	for (i=0; i<b_pgnum; i++) 
	{
		Count items = one_pg_num;
		if (i == b_pgnum-1) {
			items = p->pm - (i * one_pg_num);
		}
		// printf("%d items: %d\n", i, items);

		Page b_page = newPage();
		setPageNitems(b_page, items);
		putPage(r->bsigf, i, b_page);
		p->bsigNpages++;
	}
	p->nbsigs = 0;
	closeRelation(r);
	return 0;
}

// check whether a relation already exists

Bool existsRelation(char *name)
{
	char fname[MAXFILENAME];
	sprintf(fname,"%s.info",name);
	File f = open(fname,O_RDONLY);
	if (f < 0)
		return FALSE;
	else {
		close(f);
		return TRUE;
	}
}

// set up a relation descriptor from relation name
// open files, reads information from rel.info

Reln openRelation(char *name)
{
	Reln r = malloc(sizeof(RelnRep));
	assert(r != NULL);
	r->infof = openFile(name,"info");
	r->dataf = openFile(name,"data");
	r->tsigf = openFile(name,"tsig");
	r->psigf = openFile(name,"psig");
	r->bsigf = openFile(name,"bsig");
	read(r->infof, &(r->params), sizeof(RelnParams));
	return r;
}

// release files and descriptor for an open relation
// copy latest information to .info file
// note: we don't write ChoiceVector since it doesn't change

void closeRelation(Reln r)
{
	// make sure updated global data is put in info file
	lseek(r->infof, 0, SEEK_SET);
	int n = write(r->infof, &(r->params), sizeof(RelnParams));
	assert(n == sizeof(RelnParams));
	close(r->infof); close(r->dataf);
	close(r->tsigf); close(r->psigf); close(r->bsigf);
	free(r);
}

// insert a new tuple into a relation
// returns page where inserted
// returns NO_PAGE if insert fails completely

PageID addToRelation(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL && strlen(t) == tupSize(r));
	Page p;  PageID pid;
	RelnParams *rp = &(r->params);
	Bool isNewPage = FALSE;
	
	// add tuple to last page
	pid = rp->npages-1;
	p = getPage(r->dataf, pid);
	// check if room on last page; if not add new page
	if (pageNitems(p) == rp->tupPP) {
		isNewPage = TRUE;
		addPage(r->dataf);
		rp->npages++;
		pid++;
		free(p);
		p = newPage();
		if (p == NULL) return NO_PAGE;
	}
	addTupleToPage(r, p, t);
	rp->ntups++;  //written to disk in closeRelation()
	putPage(r->dataf, pid, p);

	/*---------------------------------------------*/
	// compute tuple signature and add to tsigf
	Bits t_sig = makeTupleSig(r, t);
	PageID t_pid = rp->tsigNpages-1;
	Page t_pg = getPage(r->tsigf, t_pid);

	if (pageNitems(t_pg) == rp->tsigPP) {
		addPage(r->tsigf);
		rp->tsigNpages++;
		t_pid++;
		free(t_pg);
		t_pg = newPage();
		if (t_pg == NULL) return NO_PAGE;
	}

	Count t_offset = pageNitems(t_pg);
	putBits(t_pg, t_offset, t_sig);
	addOneItem(t_pg);
	rp->ntsigs++;
	putPage(r->tsigf, t_pid, t_pg);

	/*---------------------------------------------*/
	// compute page signature and add to psigf

	Bits psig = makePageSig(r, t);
	PageID p_pid = rp->psigNpages-1;
	Page p_pg = getPage(r->psigf, p_pid);

	if (pageNitems(p_pg) == rp->psigPP) {
		addPage(r->psigf);
		rp->psigNpages++;
		p_pid++;
		free(p_pg);
		p_pg = newPage();
		if (p_pg == NULL) return NO_PAGE;
	}

	Count pm = psigBits(r);
	Bits cur_pgs = newBits(pm);
	// BUG
	Count psigOffset = pageNitems(p_pg);
	//  (rp->npages) % maxPsigsPP(r);
	//  pageNitems(p_pg)-1;
	// if (psigOffset < 0) { psigOffset = 0; }
	getBits(p_pg, psigOffset, cur_pgs);
	orBits(psig, cur_pgs);
	putBits(p_pg, psigOffset, psig);

	// setPageNitems(p_pg, iceil(rp->npages,));

	if(isNewPage) {
		addOneItem(p_pg);
	// 	printf("bbbbbbbbb: %d\n", pageNitems(p_pg));
	}

	if (isNewPage) {
		if (rp->npsigs == 0) {
			rp->npsigs++;
		}
		rp->npsigs++;
	}
	// rp->npsigs = rp->npages;
	putPage(r->psigf, p_pid, p_pg);

	/*---------------------------------------------*/
	// use page signature to update bit-slices

	Count one_pg_bytes = PAGESIZE - sizeof(int);
	Count one_pg_num = one_pg_bytes/rp->bsigSize;

	int i;
	for(i=0; i<pm-1; i++)
	{
		if (!bitIsSet(psig, i)) {
			continue;
		}

		PageID b_pid = iceil(i+1, one_pg_num) - 1;
		// printf("b_pid: %d\n", b_pid);
		Page b_pg = getPage(r->bsigf, b_pid);

		Bits b_bits = newBits(one_pg_bytes * 8);
		getBits(b_pg, 0, b_bits);
		Count offset = rp->bm * (i % one_pg_num) + pid;
		setBit(b_bits, offset);
		putBits(b_pg, 0, b_bits);
		putPage(r->bsigf, b_pid, b_pg);
	}
	rp->nbsigs = rp->npsigs;

	/*---------------------------------------------*/

	return nPages(r)-1;
}

// displays info about open Reln (for debugging)

void relationStats(Reln r)
{
	RelnParams *p = &(r->params);
	printf("Global Info:\n");
	printf("Dynamic:\n");
    printf("  #items:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
			p->ntups, p->ntsigs, p->npsigs, p->nbsigs);
    printf("  #pages:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
			p->npages, p->tsigNpages, p->psigNpages, p->bsigNpages);
	printf("Static:\n");
    printf("  tups   #attrs: %d  size: %d bytes  max/page: %d\n",
			p->nattrs, p->tupsize, p->tupPP);
	printf("  sigs   bits/attr: %d\n", p->tk);
	printf("  tsigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->tm, p->tsigSize, p->tsigPP);
	printf("  psigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->pm, p->psigSize, p->psigPP);
	printf("  bsigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->bm, p->bsigSize, p->bsigPP);
}
