#ifndef REGS_SNAPSHOT_H
#define REGS_SNAPSHOT_H

#include <stdint.h>

typedef struct registers {
    uint64_t rflags, rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15, rip;
} RegsSnapshot;

#endif