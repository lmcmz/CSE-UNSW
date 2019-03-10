// util.h ... interface to various useful functions
// part of SIMC signature files
// Functions that don't fit into one of the
//   obvious data types like File, Query, ...
// Written by John Shepherd, September 2018

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void fatal(char *msg, char *usage)
{
	if (strcmp(usage,"") != 0) fprintf(stderr,"%s\n",usage);
	if (strcmp(msg,"") != 0) fprintf(stderr,"%s\n",msg);
	exit(1);
}

int iceil(int val, int base)
{
	int ceil = val / base;
	if (val%base > 0) ceil++;
	return ceil;
}
