// page.h ... interface to functions on Pages
// part of SIMC signature files
// See pages.c for descriptions of Page type and functions
// Written by John Shepherd, September 2018

#ifndef PAGE_H
#define PAGE_H 1

typedef struct _PageRep *Page;

#include "defs.h"
#include "reln.h"
#include "tuple.h"

Page newPage();
void addPage(File);
Page getPage(File, PageID);
Status putPage(File, PageID, Page);
Byte *addrInPage(Page, int, int);
Count pageNitems(Page);
void  addOneItem(Page);

void setPageNitems(Page p, Count num);

#endif
