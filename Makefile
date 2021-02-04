CC = gcc
MAKE = make
CFLAGS = -Og -ggdb3
RELCFLAGS = -O3 -s

SUBDIRS = libamdmmio

all: dbg

libamdmmio/libamdmmio.a:
	$(MAKE) -f libamdmmio/Makefile

dbg: amdgpu-edcprobe.c libamdmmio/libamdmmio.a
	$(CC) $(CFLAGS) amdgpu-edcprobe.c libamdmmio/libamdmmio.a -o amdgpu-edcprobe

release: amdgpu-edcprobe.c libamdmmio/libamdmmio.a
	$(CC) $(RELCFLAGS) amdgpu-edcprobe.c libamdmmio/libamdmmio.a -o amdgpu-edcprobe

clean:
	rm -f amdgpu-edcprobe

clean_all: clean
	make -C libamdmmio -f Makefile clean
#  for dir in $(SUBDIRS); do \
#    $(MAKE) -C $$dir -f Makefile clean; \
#  done
