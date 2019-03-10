// reln.h ... interface to functions on Relations
// part of SIMC signature files
// See reln.c for details on Reln type and functions
// Written by John Shepherd, September 2018

#ifndef RELN_H
#define RELN_H 1

#include "defs.h"

// Relation parameters

typedef struct _RelnParams {
    // dynamic parameters
	Count  npages;     // number of data pages
	Count  ntups;      // number of tuples
	Count  tsigNpages; // number of tsig pages
	Count  ntsigs;     // number of tuple signatures (tsigs)
	Count  psigNpages; // number of psig pages
	Count  npsigs;     // number of page signatures (psigs)
	Count  bsigNpages; // number of bsig pages
	Count  nbsigs;     // number of bit-sliced sigs (bsigs)
    // fixed parameters (set at relation creation time)
	Count  nattrs;     // number of attributes
	float  pF;         // false match probability
	Count  tupsize;    // # bytes in tuples (all same size)
	Count  tupPP;      // max tuples per page
	Count  tk;         // bits set per attribute
	Count  tm;         // width of tuple signature (bits)
	Count  tsigSize;   // # bytes in tuple signature
	Count  tsigPP;     // max tuple signatures per page
	Count  pm;         // width of page signature (bits)
	Count  psigSize;   // # bytes in page signature
	Count  psigPP;     // max page signatures per page
	Count  bm;         // width of bit-slice (=maxpages)
	Count  bsigSize;   // # bytes in bit-slice
	Count  bsigPP;     // max bit-slices per page
} RelnParams;
	
// Open relation = parameters + open files

typedef struct _RelnRep {
	RelnParams params; // relation parameters
	File  infof;  // handle on info file
	File  dataf;  // handle on data file
	File  tsigf;  // handle on tuple signature file
	File  psigf;  // handle on page signature file
	File  bsigf;  // handle on bit-sliced signature file
} RelnRep;

typedef struct _RelnRep *Reln;

#include "tuple.h"
#include "page.h"

Status newRelation(char *name, Count nattrs, float pF,
				   Count tk, Count tm, Count pm, Count bm);
Reln openRelation(char *name);
void closeRelation(Reln r);
Bool existsRelation(char *name);
PageID addToRelation(Reln r, Tuple t);
void relationStats(Reln r);

// Convenience marcos

#define nAttrs(REL)      (REL)->params.nattrs
#define tupSize(REL)     (REL)->params.tupsize

#define nPages(REL)      (REL)->params.npages
#define nTuples(REL)     (REL)->params.ntups
#define maxTupsPP(REL)   (REL)->params.tupPP

#define nTsigPages(REL)  (REL)->params.tsigNpages
#define nTsigs(REL)      (REL)->params.ntsigs
#define maxTsigsPP(REL)  (REL)->params.tsigPP

#define nPsigPages(REL)  (REL)->params.psigNpages
#define nPsigs(REL)      (REL)->params.npsigs
#define maxPsigsPP(REL)  (REL)->params.psigPP

#define nBsigPages(REL)  (REL)->params.bsigNpages
#define nBsigs(REL)      (REL)->params.nbsigs
#define maxBsigsPP(REL)  (REL)->params.bsigPP

#define codeBits(REL)    (REL)->params.tk
#define tsigBits(REL)    (REL)->params.tm
#define psigBits(REL)    (REL)->params.pm
#define bsigBits(REL)    (REL)->params.bm

#define dataFile(REL)    (REL)->dataf
#define tsigFile(REL)    (REL)->tsigf
#define psigFile(REL)    (REL)->psigf
#define bsigFile(REL)    (REL)->bsigf

#define tgSize(REL)		 (REL)->params.tsigSize
#define pgSize(REL)		 (REL)->params.psigSize
#define bgSize(REL)		 (REL)->params.bsigSize

#endif
