// page.c ... functions on Pages
// part of SIMC signature files
// Reading/writing pages into buffers and manipulating contents
// Written by John Shepherd, September 2018

#include <unistd.h>
#include "defs.h"
#include "page.h"
#include "reln.h"

// internal representation of pages
struct _PageRep {
	Count nitems;  // #items in this page
	Byte items[1]; // start of data
};

// A Page is a chunk of memory containing PAGESIZE bytes
// It is implemented as a struct (nitems, items[])
// - nitems is #items in page (all items are same size)
// - items[] is a sequence of bytes containing items
// - items can be tuples, tsigs, psigs or bsigs
// - PageID values count # pages from start of file

// create a new initially empty page in memory
Page newPage()
{
	Page p = malloc(PAGESIZE);
	assert(p != NULL);
	memset(p, 0, PAGESIZE);
	return p;
}

// append a new Page to a file; return its PageID
void addPage(File f)
{
	int ok = lseek(f, 0, SEEK_END);
	assert(ok >= 0);
	Page p = newPage();
	int n = write(f, p, PAGESIZE);
	assert(n == PAGESIZE);
}

// fetch a Page from a file
// store it in a newly-allocated memory buffer

Page getPage(File f, PageID pid)
{
	//fprintf(stderr,"getPage(%d)\n",pid);
	assert(pid >= 0);
	Page p = malloc(PAGESIZE);
	assert(p != NULL);
	int ok = lseek(f, pid*PAGESIZE, SEEK_SET);
	assert(ok >= 0);
	int n = read(f, p, PAGESIZE);
	assert(n == PAGESIZE);
	return p;
}

// write a Page to a file; release allocated buffer

Status putPage(File f, PageID pid, Page p)
{
	//fprintf(stderr, "putPage(%d)\n", pid);
	assert(pid >= 0);
	int ok = lseek(f, pid*PAGESIZE, SEEK_SET);
	assert(ok >= 0);
	int n = write(f, p, PAGESIZE);
	assert(n == PAGESIZE);
	free(p);
	return 0;
}

// given a byte offset to an item in a Page
// return an absolute pointer (start addr of object)

Byte *addrInPage(Page p, int off, int size)
{
	return (Byte *)(&(p->items[0]) + size*off);
}

// manipulate page info

Count pageNitems(Page p) { return p->nitems; }
void  addOneItem(Page p) { p->nitems++; }

void setPageNitems(Page p, Count num) {
	p->nitems = num;
}

