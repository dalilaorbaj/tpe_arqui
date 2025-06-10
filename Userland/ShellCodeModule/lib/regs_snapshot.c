#include <stdint.h>
#include <library.h>

#define STDOUT 1

static void uint64ToHexString(uint64_t value, char* buffer);

static void uint64ToHexString(uint64_t value, char* buffer) {
    static const char hexChars[] = "0123456789ABCDEF";
    buffer[0] = '0';
    buffer[1] = 'x';
    
    for (int i = 15; i >= 0; i--) {
        buffer[2 + (15 - i)] = hexChars[(value >> (i * 4)) & 0xF];
    }
    buffer[18] = '\0';
}


void printRegsSnapshot(const RegsSnapshot *regs) {
    char hexBuffer[19];

    writeStr(STDOUT, "Registro de CPU actual:\n");

    writeStr(STDOUT, "RAX: ");
    uint64ToHexString(regs->rax, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "  RBX: ");
    uint64ToHexString(regs->rbx, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "\n");

    writeStr(STDOUT, "RCX: ");
    uint64ToHexString(regs->rcx, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "  RDX: ");
    uint64ToHexString(regs->rdx, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "\n");

    writeStr(STDOUT, "RSI: ");
    uint64ToHexString(regs->rsi, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "  RDI: ");
    uint64ToHexString(regs->rdi, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "\n");

    writeStr(STDOUT, "RBP: ");
    uint64ToHexString(regs->rbp, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "  RSP: ");
    uint64ToHexString(regs->rsp, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "\n");

    writeStr(STDOUT, "R8 : ");
    uint64ToHexString(regs->r8, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "  R9 : ");
    uint64ToHexString(regs->r9, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "\n");

    writeStr(STDOUT, "R10: ");
    uint64ToHexString(regs->r10, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "  R11: ");
    uint64ToHexString(regs->r11, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "\n");

    writeStr(STDOUT, "R12: ");
    uint64ToHexString(regs->r12, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "  R13: ");
    uint64ToHexString(regs->r13, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "\n");

    writeStr(STDOUT, "R14: ");
    uint64ToHexString(regs->r14, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "  R15: ");
    uint64ToHexString(regs->r15, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "\n");

    writeStr(STDOUT, "RIP: ");
    uint64ToHexString(regs->rip, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "  RFLAGS: ");
    uint64ToHexString(regs->rflags, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "\n");

    writeStr(STDOUT, "CS: ");
    uint64ToHexString(regs->cs, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "   SS: ");
    uint64ToHexString(regs->ss, hexBuffer);
    writeStr(STDOUT, hexBuffer);
    writeStr(STDOUT, "\n");
}