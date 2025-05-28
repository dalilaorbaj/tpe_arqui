#include "regs_snapshot.h"
#include "naiveConsole.h"
#include <stdint.h>


// Imprime todos los registros del snapshot recibido
void printRegsSnapshot(const RegsSnapshot *regs) {
    ncPrint("Registro de CPU actual:\n");
    ncPrint("RAX: "); ncPrintHex(regs->rax); ncPrint("  RBX: "); ncPrintHex(regs->rbx); ncNewline();
    ncPrint("RCX: "); ncPrintHex(regs->rcx); ncPrint("  RDX: "); ncPrintHex(regs->rdx); ncNewline();
    ncPrint("RSI: "); ncPrintHex(regs->rsi); ncPrint("  RDI: "); ncPrintHex(regs->rdi); ncNewline();
    ncPrint("RBP: "); ncPrintHex(regs->rbp); ncPrint("  RSP: "); ncPrintHex(regs->rsp); ncNewline();
    ncPrint("R8 : "); ncPrintHex(regs->r8 ); ncPrint("  R9 : "); ncPrintHex(regs->r9 ); ncNewline();
    ncPrint("R10: "); ncPrintHex(regs->r10); ncPrint("  R11: "); ncPrintHex(regs->r11); ncNewline();
    ncPrint("R12: "); ncPrintHex(regs->r12); ncPrint("  R13: "); ncPrintHex(regs->r13); ncNewline();
    ncPrint("R14: "); ncPrintHex(regs->r14); ncPrint("  R15: "); ncPrintHex(regs->r15); ncNewline();
    ncPrint("RIP: "); ncPrintHex(regs->rip); ncPrint("  RFLAGS: "); ncPrintHex(regs->rflags); ncNewline();
    ncPrint("CS : "); ncPrintHex(regs->cs ); ncPrint("  SS: "); ncPrintHex(regs->ss ); ncNewline();
}