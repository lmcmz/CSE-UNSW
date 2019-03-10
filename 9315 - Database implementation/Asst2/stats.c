// stats.c ... show statistics for a Relation
// part of SIMC signature files
// Show info and page stats for a Relation
// Usage:  ./stats  RelName

#include "defs.h"
#include "reln.h"
#include "page.h"
#include "tsig.h"

#define USAGE "./stats  RelName"

// Main ... process args, run query

int main(int argc, char **argv)
{
	// process command-line args

	if (argc < 2) fatal(USAGE, "");
	char *relname = argv[1];

	// open relation and show stats

	Reln r = openRelation(relname);
	if (r == NULL) fatal(USAGE,"No such relation");
	relationStats(r);
#if 0
	int pid, nP; Page p;
    // useful for sanity-checking contents of pages
	nP = nPages(r);
	printf("\nData Pages  [pid]:#tups");
	for (pid = 0; pid < nP; pid++) {
		if (pid%6 == 0) putchar('\n');
		p = getPage(dataFile(r), pid);
		printf(" [%02d]:%3d", pid, pageNitems(p));
		free(p);
	}
	printf("\nTsig Pages  [pid]:#sigs");
	nP = nTsigPages(r);
	for (pid = 0; pid < nP; pid++) {
		if (pid%6 == 0) putchar('\n');
		p = getPage(tsigFile(r), pid);
		printf(" [%02d]:%3d", pid, pageNitems(p));
		free(p);
	}
	putchar('\n');
#endif
	closeRelation(r);
	return 0;
}
