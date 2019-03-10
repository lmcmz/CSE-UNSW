// hash.c ... hash function (from PostgreSQL)
// part of superimposed codeword signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "hash.h"

#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c, 4); \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

Word hash_any(char *k, int keylen)
{
	Word a, b, c, len;
	/* set up the internal state */
	len = keylen;
	a = b = 0x9e3779b9;
	c = 3923095;

	/* handle most of the key */
	while (len >= 12)
	{
#ifdef WORDS_BIGENDIAN
		a += (k[3] + ((Word) k[2] << 8) + ((Word) k[1] << 16) + ((Word) k[0] << 24));
		b += (k[7] + ((Word) k[6] << 8) + ((Word) k[5] << 16) + ((Word) k[4] << 24));
		c += (k[11] + ((Word) k[10] << 8) + ((Word) k[9] << 16) + ((Word) k[8] << 24));
#else							/* !WORDS_BIGENDIAN */
		a += (k[0] + ((Word) k[1] << 8) + ((Word) k[2] << 16) + ((Word) k[3] << 24));
		b += (k[4] + ((Word) k[5] << 8) + ((Word) k[6] << 16) + ((Word) k[7] << 24));
		c += (k[8] + ((Word) k[9] << 8) + ((Word) k[10] << 16) + ((Word) k[11] << 24));
#endif   /* WORDS_BIGENDIAN */
		mix(a, b, c);
		k += 12;
		len -= 12;
	}

	/* handle the last 11 bytes */
#ifdef WORDS_BIGENDIAN
	switch (len)			/* all the case statements fall through */
	{
		case 11: c += ((Word) k[10] << 8);
		case 10: c += ((Word) k[9] << 16);
		case 9: c += ((Word) k[8] << 24);
			/* the lowest byte of c is reserved for the length */
		case 8: b += k[7];
		case 7: b += ((Word) k[6] << 8);
		case 6: b += ((Word) k[5] << 16);
		case 5: b += ((Word) k[4] << 24);
		case 4: a += k[3];
		case 3: a += ((Word) k[2] << 8);
		case 2: a += ((Word) k[1] << 16);
		case 1: a += ((Word) k[0] << 24);
		/* case 0: nothing left to add */
	}
#else							/* !WORDS_BIGENDIAN */
	switch (len)			/* all the case statements fall through */
	{
		case 11: c += ((Word) k[10] << 24);
		case 10: c += ((Word) k[9] << 16);
		case 9: c += ((Word) k[8] << 8);
			/* the lowest byte of c is reserved for the length */
		case 8: b += ((Word) k[7] << 24);
		case 7: b += ((Word) k[6] << 16);
		case 6: b += ((Word) k[5] << 8);
		case 5: b += k[4];
		case 4: a += ((Word) k[3] << 24);
		case 3: a += ((Word) k[2] << 16);
		case 2: a += ((Word) k[1] << 8);
		case 1: a += k[0];
		/* case 0: nothing left to add */
	}
#endif   /* WORDS_BIGENDIAN */

	final(a, b, c);
	return c;
}
