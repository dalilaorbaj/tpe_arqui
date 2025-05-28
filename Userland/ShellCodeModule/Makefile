include ../Makefile.inc

MODULE=0000-shell.bin
SOURCES=$(wildcard Main/*.c) ./lib/naiveConsole.c ./lib/library.c ../syscalls.c ./lib/regs_snapshot.c
OBJS=$(SOURCES:.c=.o)
ASM_OBJS=../syscalls.o ./lib/invopcode.o ./lib/functions.o

all: $(MODULE)

../syscalls.o: ../syscalls.asm
	$(ASM) $(ASMFLAGS) $< -o $@

./lib/invopcode.o: ./lib/invopcode.asm
	$(ASM) $(ASMFLAGS) $< -o $@

./lib/functions.o: ./lib/functions.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(MODULE): _loader.c $(SOURCES) shell.ld $(ASM_OBJS)
	$(GCC) $(GCCFLAGS) -I./include -T shell.ld _loader.c $(SOURCES) $(ASM_OBJS) -o ../$(MODULE)

clean:
	rm -rf Main/*.o lib/*.o *.o ../syscalls.o

.PHONY: all clean print