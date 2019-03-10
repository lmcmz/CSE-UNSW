// gendata.c ... generate random tuples
// part of superimposed codeword signature files
// Generates a list of K random tuples with N attributes
// Note: all tuples have the same structure:
// 7-digits,20-alphas,10-alphanum,..., up to N attributes
// Usage:  ./insert  [-v]  #attributes  #tuples
// Written by John Shepherd, September 2018

#include "defs.h"

#define USAGE "./gendata  #tuples  #attributes  [startID]  [seed]"

// Main ... process args, read/insert tuples

int main(int argc, char **argv)
{
	int  natts;	// number of attributes in each tuple
	int  ntups;	// number of tuples
	int  startID;  // starting ID
	char err[100]; // buffer for error messages
	char *randWord(char *buf, int nchars);

	// process command-line args

	if (argc < 3) fatal(USAGE,"");

	// how many tuples
	ntups = atoi(argv[1]);
	if (ntups < 1 || ntups > 100000) {
		sprintf(err, "Invalid #tuples: %d (must be 0 < # < 10^6)", ntups);
		fatal("", err);
	}

	// how many attributes in each tuple
	natts = atoi(argv[2]);
	if (natts < 2 || natts > 9) {
		sprintf(err, "Invalid #attrs: %d (must be 1 < # < 10)", natts);
		fatal("", err);
	}

	// set starting ID
	if (argc < 4)
		startID = 1000000;
	else {
		startID = atoi(argv[3]);
		if (startID < 0 || startID > 9000000) {
			sprintf(err, "Invalid startID: %d (must be 0 <= # < 9000000)", startID);
			fatal("", err);
		}
	}

	// seed random # generator
	if (argc < 5)
		srandom(0);
	else
		srandom(atoi(argv[4]));

	// id ensures that all tuples are distinct
	int i, j, id=startID;
	for (i = 0; i < ntups; i++) {
		char val[30];
		printf("%07d,%s", id++, randWord(val, 20));
		for (j = 0; j < natts-2; j++) {
			int period = (j+3)*83; // max period = 996
			printf(",a%d-%03d", j+3, (i%period));
		}
		putchar('\n');
	}

	return OK;
}

// randWord: generate random sequence of Nchars alphas or alphanums
// - store in supplied buffer, which hopefully is long enough

char alpha[52] = {
	'a','b','c','d','e','f','g','h','i','j','k','l','m',
	'n','o','p','q','r','s','t','u','v','w','x','y','z',
	'A','B','C','D','E','F','G','H','I','J','K','L','M',
	'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
};

char *randWord(char *buf, int nchars)
{
	int i;
	for (i = 0; i < nchars; i++) {
		buf[i] = alpha[rand()%52];
	}
	buf[i] = '\0';
	return buf;
}
