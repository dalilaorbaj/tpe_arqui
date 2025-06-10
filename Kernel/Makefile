# include Makefile.inc

# KERNEL=kernel.bin
# SOURCES=$(wildcard *.c drivers/*.c interruptions/*.c utils/*.c)
# SOURCES_ASM=$(wildcard asm/*.asm)
# OBJECTS=$(SOURCES:.c=.o)
# OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
# LOADERSRC=loader.asm
# LOADEROBJECT=$(LOADERSRC:.asm=.o)
# STATICLIBS=

# all: $(KERNEL)

# $(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
# 	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

# %.o: %.c
# 	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

# %.o : %.asm
# 	$(ASM) $(ASMFLAGS) $< -o $@

# $(LOADEROBJECT):
# 	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

# clean:
# 	rm -rf asm/*.o *.o *.bin interruptions/*.o drivers/*.o

# .PHONY: all clean


include Makefile.inc

KERNEL      := kernel.bin
SRC_DIRS    := . drivers interruptions utils
ASM_DIRS    := asm
C_SRCS      := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
ASM_SRCS    := $(foreach dir,$(ASM_DIRS),$(wildcard $(dir)/*.asm))
OBJS        := $(C_SRCS:.c=.o)
ASM_OBJS    := $(ASM_SRCS:.asm=.o)
LOADERSRC   := loader.asm
LOADEROBJ   := $(LOADERSRC:.asm=.o)
STATICLIBS  :=

INCLUDES    := -I./include

all: $(KERNEL)

$(KERNEL): $(LOADEROBJ) $(OBJS) $(ASM_OBJS) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^ $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJ): $(LOADERSRC)
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf $(foreach dir,$(SRC_DIRS),$(dir)/*.o) $(foreach dir,$(ASM_DIRS),$(dir)/*.o) *.o *.bin

.PHONY: all clean