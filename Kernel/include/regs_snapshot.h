#ifndef REGS_SNAPSHOT_H
#define REGS_SNAPSHOT_H

#include <stdint.h>

typedef struct registers {
    uint64_t rflags, rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15, rip, cs, ss;
} RegsSnapshot;

//(!) no se si va 18 o 20
#define CANT_REGS 18

extern int64_t registersArray[CANT_REGS];
extern int64_t registersArrayAux[CANT_REGS];
extern int64_t registersArrayException[CANT_REGS];


#endif