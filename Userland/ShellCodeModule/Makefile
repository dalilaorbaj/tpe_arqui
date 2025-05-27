include ../Makefile.inc

MODULE=0000-shell.bin
SOURCES=$(wildcard Main/*.c) ../lib/naiveConsole.c ../lib/library.c


all: $(MODULE)

$(MODULE): _loader.c $(SOURCES) shell.ld
	$(GCC) $(GCCFLAGS) -T shell.ld _loader.c $(SOURCES) -o ../$(MODULE)

clean:
	rm -rf *.o

.PHONY: all clean print
