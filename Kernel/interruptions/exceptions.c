#include <stdint.h>
#include <video.h>
#include <idtLoader.h>
#include <syscalls.h>
#include <keyboard.h>

#define DEFAULT_MASTER_MASK 0xFC

extern int64_t registersArrayException[CANT_REGS];

#define ZERO_EXCEPTION_ID 0
#define OPCODE_EXCEPTION_ID 6

extern void _hlt();
extern void _sti();
extern void _cli();

static void printRegs(char * message, uint8_t cant_chars_message);
static char * numToString(uint64_t num, uint64_t base);
static uint64_t strlen(const char *s);
static void flushKeyboardBuffer();


static void flushKeyboardBuffer() {
    uint16_t buffer;
    uint64_t nbytes;
    
    while ((nbytes = sys_read(0, &buffer, 1)) > 0) {
        //descartamos el valor leído
    }
}


void waitToReturn() {
    getKey();
}

static uint64_t strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return (uint64_t)(p - s);
}


void exceptionDispatcher(uint64_t exception) {
    char * message = "";
    uint8_t dim = 0;
	

    if (exception == ZERO_EXCEPTION_ID) {
        message = "Zero div exception\n\n";
        dim = strlen((char*)message);
    } else if (exception == OPCODE_EXCEPTION_ID) {
        message = "Invalid opcode\n\n";
        dim = strlen((char*)message);
    }

	resetScreen((Color){139,0,36});
    printRegs(message, dim);

    const char *promo = "\nPress any letter key to return to shell\n";
    write((const char *)promo, strlen(promo), (Color){255, 255, 255});

    picMasterMask(0xFD);    // Solo IRQ1 (teclado)
    _sti();
    flushKeyboardBuffer();
    waitToReturn();           // se bloquea c/ hlt hasta recibir un byte
    _cli();
    picMasterMask(DEFAULT_MASTER_MASK);
	flushKeyboardBuffer();
    empty_screen((Color){0,0,0});
}

static char * numToString(uint64_t num, uint64_t base) {
    static char buffer[64];
    char * ptr = &buffer[63];
    *ptr = '\0';
    do {
        *--ptr = "0123456789abcdef"[num % base];
        num /= base;
    } while(num != 0);
    return ptr;
}

static void printRegs(char * message, uint8_t cant_chars_message){
    char * regs[] = {"RFLAGS", "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "CS", "SS"};
    empty_screen((Color){139,0,36});

    write(message, cant_chars_message, (Color){255,255,255});
    const char newline = '\n';
    for(int i=0; i<CANT_REGS;i++){
        write(regs[i], 10, (Color){255,255,255});
        write(": 0x", 4, (Color){255,255,255});
        write(numToString(registersArrayException[i],16), 10, (Color){255,255,255});
        write(&newline, 1, (Color){255,255,255});
    }
}