
CC=gcc
CCOPT=-c -Wall
CCSHAREDOPT=-fpic
CCDEBUGOPT=-g
CSHARED=-shared -Wl,-soname

static: liblzop.o lzop_support.o

static-debug: liblzop-d.o lzop_support-d.o

liblzop.o: liblzop.c lzop.h lzop_support.h version.h
	$(CC) $(CCOPT) liblzop.c

lzop_support.o: lzop_support.c lzop_support.h
	$(CC) $(CCOPT) lzop_support.c

liblzop-d.o: liblzop.c lzop.h lzop_support.h version.h
	$(CC) $(CCOPT) $(CCDEBUGOPT) -o liblzop-d.o liblzop.c

lzop_support-d.o: lzop_support.c lzop_support.h
	$(CC) $(CCOPT) $(CCDEBUGOPT) -o lzop_support-d.o lzop_support.c

clean-all:
	rm -f liblzop.o lzop_support.o liblzop-d.o lzop_support-d.o
