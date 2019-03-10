// tuple.h ... interface to functions on Tuples
// part of SIMC signature files
// A Tuple is just a '\0'-terminated C string
// Consists of "val_1,val_2,val_3,...,val_n"
// See tuple.c for details on functions
// Written by John Shepherd, September 2018

#ifndef TUPLE_H
#define TUPLE_H 1

typedef char *Tuple;

#include "reln.h"
#include "page.h"

Tuple readTuple(Reln r, FILE *f);
char **tupleVals(Reln r, Tuple t);
void freeVals(char **vals, int nattrs);
Bool tupleMatch(Reln r, Tuple t1, Tuple t2);
Status addTupleToPage(Reln r, Page p, Tuple t);
Tuple getTupleFromPage(Reln r, Page p, int i);
void showTuple(Reln r, Tuple t);

#endif
