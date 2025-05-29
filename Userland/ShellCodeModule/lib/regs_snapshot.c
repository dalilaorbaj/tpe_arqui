#include <stdint.h>
#include <library.h>


// Helper function to convert uint64_t to hex string
static void uint64ToHexString(uint64_t value, char* buffer) {
    const char hexChars[] = "0123456789ABCDEF";
    buffer[0] = '0';
    buffer[1] = 'x';
    
    for (int i = 15; i >= 0; i--) {
        buffer[2 + (15 - i)] = hexChars[(value >> (i * 4)) & 0xF];
    }
    buffer[18] = '\0';
}

// Imprime todos los registros del snapshot recibido
void printRegsSnapshot(const RegsSnapshot *regs) {
    char hexBuffer[19]; // "0x" + 16 hex digits + null terminator
    
    puts("Registro de CPU actual:\n");
    
    puts("RAX: ");
    uint64ToHexString(regs->rax, hexBuffer);
    puts(hexBuffer);
    puts("  RBX: ");
    uint64ToHexString(regs->rbx, hexBuffer);
    puts(hexBuffer);
    puts("\n");
    
    puts("RCX: ");
    uint64ToHexString(regs->rcx, hexBuffer);
    puts(hexBuffer);
    puts("  RDX: ");
    uint64ToHexString(regs->rdx, hexBuffer);
    puts(hexBuffer);
    puts("\n");
    
    puts("RSI: ");
    uint64ToHexString(regs->rsi, hexBuffer);
    puts(hexBuffer);
    puts("  RDI: ");
    uint64ToHexString(regs->rdi, hexBuffer);
    puts(hexBuffer);
    puts("\n");
    
    puts("RBP: ");
    uint64ToHexString(regs->rbp, hexBuffer);
    puts(hexBuffer);
    puts("  RSP: ");
    uint64ToHexString(regs->rsp, hexBuffer);
    puts(hexBuffer);
    puts("\n");
    
    puts("R8 : ");
    uint64ToHexString(regs->r8, hexBuffer);
    puts(hexBuffer);
    puts("  R9 : ");
    uint64ToHexString(regs->r9, hexBuffer);
    puts(hexBuffer);
    puts("\n");
    
    puts("R10: ");
    uint64ToHexString(regs->r10, hexBuffer);
    puts(hexBuffer);
    puts("  R11: ");
    uint64ToHexString(regs->r11, hexBuffer);
    puts(hexBuffer);
    puts("\n");
    
    puts("R12: ");
    uint64ToHexString(regs->r12, hexBuffer);
    puts(hexBuffer);
    puts("  R13: ");
    uint64ToHexString(regs->r13, hexBuffer);
    puts(hexBuffer);
    puts("\n");
    
    puts("R14: ");
    uint64ToHexString(regs->r14, hexBuffer);
    puts(hexBuffer);
    puts("  R15: ");
    uint64ToHexString(regs->r15, hexBuffer);
    puts(hexBuffer);
    puts("\n");
    
    puts("RIP: ");
    uint64ToHexString(regs->rip, hexBuffer);
    puts(hexBuffer);
    puts("  RFLAGS: ");
    uint64ToHexString(regs->rflags, hexBuffer);
    puts(hexBuffer);
    puts("\n");
    
    puts("CS : ");
    uint64ToHexString(regs->cs, hexBuffer);
    puts(hexBuffer);
    puts("  SS: ");
    uint64ToHexString(regs->ss, hexBuffer);
    puts(hexBuffer);
    puts("\n");
}