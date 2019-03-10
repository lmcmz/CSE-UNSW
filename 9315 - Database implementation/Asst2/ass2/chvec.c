// chvec.c ... functions on ChoiceVectors
// part of Multi-attribute Linear-hashed Files
// A ChVec is an array of MAXCHVEC ChVecItems
// Each ChVecItem is a pair (attr#,bit#)
// See chvec.c for details on functions
// Written by John Shepherd, April 2016

#include "defs.h"
#include "reln.h"
#include "chvec.h"

// convert a a,b:a,b:a,b:...:a,b" representation
//  of a choice vector into a ChVec
// if string doesn't specify all 32 bits, then
//  cycle through attributes until reach 32 bits

Status parseChVec(Reln r, char *str, ChVec cv)
{
	Count i = 0, nattr = nattrs(r);
	char *c = str, *c0 = str;
	while (*c != '\0') {
		while (*c != ':' && *c != '\0') c++;
		Count a, b, n;
		if (*c == '\0') {
			n = sscanf(c0, "%d,%d", &a, &b);
			// is the (attr,bit) pair valid?
			if (n != 2 || a >= nattr || b >= 32) {
				printf("Invalid choice vector\n");
				return ~OK;
			}
		}
		else {
			*c = '\0';
			n = sscanf(c0, "%d,%d", &a, &b);
			if (n != 2 || a >= nattr || b >= 32) {
				printf("Invalid choice vector\n");
                return ~OK;
            }
			*c = ':'; c++; c0 = c;
		}
		cv[i].att = a; cv[i].bit = b;
		printf("cv[%d] is (%d,%d)\n", i, cv[i].att, cv[i].bit);
		i++;
	}
	// get enough bits for a 32-bit choice vector
	// take new bits from top end of each hash,
	//   so as to hopefully not conflict 
	Count x;  Count next[MAXCHVEC];
	for (x = 0; x < MAXCHVEC; x++) next[x] = 31;
	x = 0;
	while (i < MAXCHVEC) {
		cv[i].att = x; cv[i].bit = next[x];
		printf("cv[%d] is (%d,%d)\n", i, cv[i].att, cv[i].bit);
		next[x]--;
		i++; x = (x+1) % nattr;
	}
	return OK;
}

// print a choice vector (for debugging)

void printChVec(ChVec cv)
{
	int i;
	for (i = 0; i < MAXCHVEC; i++) {
		printf("%d,%d",cv[i].att, cv[i].bit);
		if (i < MAXCHVEC-1) putchar(':');
	}
	printf("\n");
}
