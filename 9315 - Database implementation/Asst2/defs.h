// defs.h ... global definitions
// part of SIMC signature files
// Defines types and constants used throughout code
// Written by John Shepherd, September 2018

#ifndef DEFS_H
#define DEFS_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "util.h"

#define PAGESIZE	4096
#define NO_PAGE	 0xffffffff
#define MAXERRMSG   200
#define MAXTUPLEN   200
#define MAXRELNAME  200
#define MAXFILENAME MAXRELNAME+8
#define OK		  0
#define NOT_OK	  0xffffffff
#define FALSE	   0
#define TRUE		1

typedef char Bool;
typedef unsigned char Byte;
typedef int Status;
typedef int File;
typedef unsigned int Offset;
typedef unsigned int Count;
typedef unsigned int Word;
typedef Offset PageID;

#endif
