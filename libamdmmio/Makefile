CC 			= gcc
AR 			= ar
CFLAGS 		= -Og -Ilibamdmmio -fPIC -ggdb3
RELCFLAGS 	= -O3 -Ilibamdmmio -fPIC -s

all: dbg

dbg: libamdmmio/amdmmio.c libamdmmio/amdmmio.h libamdmmio/amdmmioregs.h
	$(CC) $(CFLAGS) -c libamdmmio/amdmmio.c -o libamdmmio/amdmmio.o
	$(AR) rcs libamdmmio/libamdmmio.a libamdmmio/amdmmio.o

release: libamdmmio/amdmmio.c libamdmmio/amdmmio.h libamdmmio/amdmmioregs.h
	$(CC) $(RELCFLAGS) -c libamdmmio/amdmmio.c -o libamdmmio/amdmmio.o
	$(AR) rcs libamdmmio/libamdmmio.a libamdmmio/amdmmio.o

clean:
	rm -f *.o *.a
