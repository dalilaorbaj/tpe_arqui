#include <stdint.h>
#include <video.h>
#include <idtLoader.h>
#include <syscalls.h>

#define STDERR 1

#define DEFAULT_MASTER_MASK 0xFC

#define CANT_REGS 18
extern int64_t registersArrayException[CANT_REGS];

#define ZERO_EXCEPTION_ID 0
#define OPCODE_EXCEPTION_ID 6

extern void _hlt();
extern void _sti();
extern void _cli();

static void printRegs(uint8_t * message, uint8_t cant_chars_message);
static uint8_t * numToString(uint64_t num, uint64_t base);
static uint64_t strlen(const char *s);
static void flushKeyboardBuffer();


static void flushKeyboardBuffer() {
    uint8_t buffer;
    uint64_t nbytes;
    
    // Lee y descarta todas las teclas acumuladas en el buffer
    while ((nbytes = sys_read(0, &buffer, 1)) > 0) {
        // Simplemente descartamos el valor leído
        // El bucle continúa hasta que no haya más datos en el buffer
    }
}

// void waitToReturn() {
//     uint8_t c = 0;
//     do {
//         _hlt();
//     } while (sys_read(0, &c, 1) <= 0);
// }

//(!) PROBLEMA: TE SACA DE LA PANTALLA PERO DEJA EN LA TERMINAL LA TECLA QUE USASTE PARA SALIR
void waitToReturn() {
    getKey();
}

static uint64_t strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return (uint64_t)(p - s);
}


void exceptionDispatcher(uint64_t exception) {
    uint8_t * message = "";
    uint8_t dim = 0;

    if (exception == ZERO_EXCEPTION_ID) {
        message = "Zero div exception\n\n";
        dim = strlen((char*)message);
    } else if (exception == OPCODE_EXCEPTION_ID) {
        message = "Invalid opcode\n\n";
        dim = strlen((char*)message);
    }

    empty_screen((Color){139,0,36});
    printRegs(message, dim);

    const char *promo = "\nPress any key to return to shell\n";
    write((uint8_t*)promo, strlen(promo), (Color){255, 255, 255});

    // 4) Habilitar teclado y esperar la primera pulsación
    picMasterMask(0xFD);    // Solo IRQ1 (teclado)
    _sti();
    flushKeyboardBuffer();
    waitToReturn();           // se bloqueará c/ hlt hasta recibir un byte
    _cli();
    picMasterMask(DEFAULT_MASTER_MASK);

    // 5) “Opcional”: Si deseas limpiar la pantalla ANTES de volver, hazlo aquí:
    // empty_screen((Color){255,255,255});

    // 6) Finalmente retornas con iretq o bien sigues tu flujo de error
    // (aca podrías invocar un iretq o un long jump a tu manejador de recuperación).
}

static uint8_t * numToString(uint64_t num, uint64_t base) {
    static char buffer[64];
    uint8_t * ptr = &buffer[63];
    *ptr = '\0';
    do {
        *--ptr = "0123456789abcdef"[num % base];
        num /= base;
    } while(num != 0);
    return ptr;
}

//(!)hacerla diferente
static void printRegs(uint8_t * message, uint8_t cant_chars_message){
    uint8_t * regs[] = {"RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RFLAGS"};
    empty_screen((Color){139,0,36});

    write(message, cant_chars_message, (Color){255,255,255});
    uint8_t newline = '\n';
    for(int i=0; i<CANT_REGS;i++){
        write(regs[i], 10, (Color){255,255,255});
        write(": 0x", 4, (Color){255,255,255});
        write(numToString(registersArrayException[i],16), 10, (Color){255,255,255});
        write("h", 1, (Color){255,255,255});
        write(&newline, 1, (Color){255,255,255});
    }
}