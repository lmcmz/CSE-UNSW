# Makefile for COMP9315 18s2 Assignment 2

CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror -g
LIBS=query.o page.o reln.o tuple.o util.o tsig.o psig.o bsig.o hash.o bits.o
BINS=create insert select stats gendata dump x1 x2 x3

all : $(LIBS) $(BINS)

create: create.o reln.o tuple.o page.o util.o
	gcc -o create create.o $(LIBS) -lm
insert: insert.o $(LIBS)
select: select.o $(LIBS)
stats:  stats.o $(LIBS)
dump: dump.o $(LIBS)
gendata: gendata.o util.o
	gcc -o gendata gendata.o util.o -lm

create.o: create.c defs.h
insert.o: insert.c defs.h reln.h tuple.h
select.o: select.c defs.h query.h tuple.h reln.h hash.h bits.h
stats.o: stats.c defs.h reln.h page.h
gendata.o: gendata.c defs.h
dump.o: dump.c defs.h tuple.h reln.h

bits.o: bits.c bits.h defs.h page.h
hash.o: hash.c defs.h hash.h bits.h
page.o: page.c defs.h bits.h
query.o: query.c defs.h query.h reln.h tuple.h
reln.o: reln.c defs.h reln.h page.h tuple.h hash.h bits.h
tsig.o: tsig.c defs.h reln.h page.h tsig.h bits.h
psig.o: psig.c defs.h reln.h page.h psig.h bits.h
bsig.o: bsig.c defs.h reln.h page.h bsig.h bits.h
tuple.o: tuple.c defs.h tuple.h reln.h hash.h bits.h
util.o: util.c

defs.h: util.h

x1 : x1.o bits.o page.o
	$(CC) -o x1 x1.o $(LIBS)

x2 : x2.o reln.o page.o tuple.o tsig.o bits.o hash.o query.o
	$(CC) -o x2 x2.o $(LIBS)

x3 : x3.o reln.o page.o tuple.o tsig.o bits.o hash.o
	$(CC) -o x3 x3.o $(LIBS)


db:
	rm -f R.*
	./create R 3 5 ""
	./gendata 1000 3 1234 | ./insert R

clean:
	rm -f $(BINS) *.o
