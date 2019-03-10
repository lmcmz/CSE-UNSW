// chvec.h ... interface to functions on ChoiceVectors
// part of Multi-attribute Linear-hashed Files
// A ChVec is an array of MAXCHVEC ChVecItems
// Each ChVecItem is a pair (attr#,bit#)
// See chvec.c for details on functions
// Written by John Shepherd, April 2016

#ifndef CHVEC_H
#define CHVEC_H 1

#include "defs.h"
#include "reln.h"

#define MAXCHVEC 32

typedef struct {
	Byte att;
	Byte bit;
} ChVecItem;

typedef ChVecItem ChVec[MAXCHVEC];

Status parseChVec(Reln r, char *str, ChVec cv);
void printChVec(ChVec cv);

#endif
