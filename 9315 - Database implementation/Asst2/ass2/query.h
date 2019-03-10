// query.h ... interface to query scan functions
// part of SIMC signature files
// See query.c for details of Query type and functions
// Written by John Shepherd, September 2018

#ifndef QUERY_H
#define QUERY_H 1

#include "reln.h"
#include "tuple.h"
#include "bits.h"

// A suggestion ... you can change however you like

typedef struct _QueryRep {
	// static info
	Reln    rel;       // need to remember Relation info
	char   *qstring;   // query string
	//dynamic info
	Bits    pages;     // list of pages to examine
	PageID  curpage;   // current page in scan
	Count   curtup;    // current tuple within page
	// statistics info
	Count   nsigs;     // how many signatures read
	Count   nsigpages; // how many signature pages read
	Count   ntuples;   // how many tuples examined
	Count   ntuppages; // how many data pages read
	Count   nfalse;    // how many pages had no matching tuples
} QueryRep;

typedef struct _QueryRep *Query;

Query startQuery(Reln, char *, char);
void  scanAndDisplayMatchingTuples(Query);
void  queryStats(Query);
void  closeQuery(Query);

#endif
