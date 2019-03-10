// dump.c ... display all tuples
// part of SIMC signature files
// Usage:  ./dump  RelName 

#include "defs.h"
#include "tuple.h"
#include "reln.h"

#define USAGE "./dump  RelName"

// Main ... process args, run query

int main(int argc, char **argv)
{
	Reln r;  // open relation info
	char err[MAXERRMSG];  // buffer for error messages

	// process command-line args

	if (argc < 2) fatal(USAGE, "");

	if ((r = openRelation(argv[1])) == NULL) {
		sprintf(err, "Can't open relation: %s",argv[1]);
		fatal("", err);
	}

	// scan data pages, printing each tuple

	Page p; Tuple t; int pid, i;
	for (pid = 0; pid < nPages(r); pid++) {
		p = getPage(dataFile(r), pid);
		for (i = 0; i < pageNitems(p); i++) {
			t = getTupleFromPage(r, p, i);
			showTuple(r, t);
			free(t);
		}
		free(p);
	}

	// clean up

	closeRelation(r);
	return 0;
}

