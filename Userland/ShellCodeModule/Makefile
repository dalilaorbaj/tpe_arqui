include ../Makefile.inc

MODULE      := 0000-shell.bin
SRC_DIRS    := Main Modules lib
C_SRCS      := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
ASM_SRCS    := ../syscallsHelper.asm lib/functions.asm
OBJS        := $(C_SRCS:.c=.o)
ASM_OBJS    := $(ASM_SRCS:.asm=.o)

INCLUDES    := -I./include

all: $(MODULE)

%.o: %.c
	$(GCC) $(GCCFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(MODULE): _loader.c $(C_SRCS) shell.ld $(ASM_OBJS)
	$(GCC) $(GCCFLAGS) $(INCLUDES) -T shell.ld _loader.c $(C_SRCS) $(ASM_OBJS) -o ../$(MODULE)

clean:
	rm -rf $(foreach dir,$(SRC_DIRS),$(dir)/*.o) *.o ../syscallsHelper.o lib/functions.o

.PHONY: all clean print