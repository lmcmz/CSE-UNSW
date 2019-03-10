// bits.c ... functions on bit-strings
// part of SIMC signature files
// Bit-strings are arbitrarily long byte arrays
// Least significant bits (LSB) are in array[0]
// Most significant bits (MSB) are in array[nbytes-1]

// Written by John Shepherd, September 2018

#include <assert.h>
#include "defs.h"
#include "bits.h"
#include "page.h"

typedef struct _BitsRep {
	Count  nbits;		  // how many bits
	Count  nbytes;		  // how many bytes in array
	Byte   bitstring[1];  // array of bytes to hold bits
	                      // actual array size is nbytes
} BitsRep;

// create a new Bits object

Bits newBits(int nbits)
{
	Count nbytes = iceil(nbits,8);
	Bits new = malloc(2*sizeof(Count) + nbytes);
	new->nbits = nbits;
	new->nbytes = nbytes;
	memset(&(new->bitstring[0]), 0, nbytes);
	return new;
}

// Check Here: https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit

// release memory associated with a Bits object

void freeBits(Bits b)
{
	// free(b->bitstring);
	free(b);
}
// check if the bit at position is 1

Bool bitIsSet(Bits b, int position)
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	int byte_num = position / 8;
	int offset = position % 8;
	Byte byte = b->bitstring[byte_num];
	return ((byte >> offset) & 1);
}

// check whether one Bits b1 is a subset of Bits b2

Bool isSubset(Bits b1, Bits b2)
{
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	
	int i;
	for(i=0; i<b1->nbytes; i++)
	{
		Bool result = FALSE;
		Byte b1_i = b1->bitstring[i];
		Byte b2_i = b2->bitstring[i];
		if ( (b1_i | b2_i) ==  b2_i) {
			result = TRUE;
		}
		if (result == FALSE) {
			return FALSE;
		}
	}
	return TRUE;
}

// set the bit at position to 1

void setBit(Bits b, int position)
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	int byte_num = position / 8;
	int offset = position % 8;
	Byte byte = b->bitstring[byte_num];
	byte |= 1 << offset;
	b->bitstring[byte_num] = byte;
}

// set all bits to 1

void setAllBits(Bits b)
{
	assert(b != NULL);
	memset(&(b->bitstring[0]), 255, b->nbytes);
}

// set the bit at position to 0

void unsetBit(Bits b, int position)
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	int byte_num = position / 8;
	int offset = position % 8;
	Byte byte = b->bitstring[byte_num];
	byte &= ~(1 << offset);
	b->bitstring[byte_num] = byte;
}

// set all bits to 0

void unsetAllBits(Bits b)
{
	assert(b != NULL);
	memset(&(b->bitstring[0]), 0, b->nbytes);
}

// bitwise AND ... b1 = b1 & b2
 
void andBits(Bits b1, Bits b2)
{
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	int i;
	for (i=0; i<b1->nbytes; i++) {
		Byte b1_p = b1->bitstring[i];
		Byte b2_p = b2->bitstring[i];
		b1_p &= b2_p;
		b1->bitstring[i] = b1_p;
	}
}

// bitwise OR ... b1 = b1 | b2

void orBits(Bits b1, Bits b2)
{
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	int i;
	for (i=0; i<b1->nbytes; i++) {
		Byte b1_p = b1->bitstring[i];
		Byte b2_p = b2->bitstring[i];
		b1_p |= b2_p;
		b1->bitstring[i] = b1_p;
	}
}


// get a bit-string (of length b->nbytes)
// from specified position in Page buffer
// and place it in a BitsRep structure

void getBits(Page p, Offset pos, Bits b)
{
	// Count nbits = b->nbits;
	Count nbytes = b->nbytes;
	Byte *addr = addrInPage(p, pos, sizeof(Byte)*nbytes);
	int i;
	for(i=0; i<nbytes; i++)
	{
		b->bitstring[i] = *addr;
		addr++;
	}
}

// copy the bit-string array in a BitsRep
// structure to specified position in Page buffer

void putBits(Page p, Offset pos, Bits b)
{
	Count nbytes = b->nbytes;
	Byte *addr =  addrInPage(p, pos, sizeof(Byte)*nbytes);
	int i;
	for(i=0; i<nbytes; i++)
	{
		*addr = b->bitstring[i];
		addr++;
	}
}

// show Bits on stdout
// display in order MSB to LSB
// do not append '\n'

void showBits(Bits b)
{
	assert(b != NULL);
    // printf("(%d,%d)",b->nbits,b->nbytes);
	for (int i = b->nbytes-1; i >= 0; i--) {
		for (int j = 7; j >= 0; j--) {
			Byte mask = (1 << j);
			if (b->bitstring[i] & mask)
				putchar('1');
			else
				putchar('0');
		}
		putchar(' ');
	}
}

Count getBitsCount(Bits b)
{
	return b->nbits;
}

Count getBytesCount(Bits b)
{
	return b->nbytes;
}

Byte getBytesPos(Bits b, int pos)
{
	return b->bitstring[pos];
}

void setBitsCount(Count count, Bits b)
{
	b->nbits = count;
}

void setBytesCount(Count count, Bits b)
{
	b->nbytes = count;
}

