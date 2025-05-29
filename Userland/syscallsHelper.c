#include <stdint.h>
#include <stdarg.h>
#include "regs_snapshot.h"
#include "time_struct.h"
#include "naiveConsole.h"
#include "library.h"

// Declaraciones de las funciones implementadas en syscalls.asm
extern int64_t sys_read(int fd, void *buf, uint64_t count);
extern int64_t sys_write(int fd, const void *buf, uint64_t count);
extern int64_t sys_clear_screen(void);
extern int64_t sys_set_font_size(uint64_t size);
extern int64_t sys_get_registers(RegsSnapshot *snapshot);
extern int64_t sys_get_time(time_struct *t);
extern void hlt(void);
extern unsigned char sys_get_key(void);

int timeCommand() {
    time_struct t;
    if (sys_get_time(&t) == 0) {
        ncPrint("Hora actual: ");
        ncPrintDec(t.hour);
        ncPrint(":");
        if (t.minutes < 10) ncPrint("0");
        ncPrintDec(t.minutes);
        ncPrint(":");
        if (t.seconds < 10) ncPrint("0");
        ncPrintDec(t.seconds);
        ncPrint("  ");
        ncPrintDec(t.day);
        ncPrint("/");
        ncPrintDec(t.month);
        ncPrint("/");
        ncPrintDec(t.year);
        ncNewline();
        return 0;
    } else {
        ncPrint("Error al obtener la hora\n");
    }
    return ERROR;
}

// void hlt(void) {
//     hlt();
// }

// Implementación de getKey (stub, deberías hacer syscall real si tu kernel la soporta)
unsigned char getKey() {
    return sys_get_key();
}

// Implementación de writeStr usando sys_write
int64_t writeStr(int fd, const char *s) {
    uint64_t len = 0;
    while (s[len]) len++;
    return sys_write(fd, s, len);
}

// Implementación mínima de fprintf (solo imprime el formato)
int64_t fprintf(int fd, const char *fmt, va_list args) {
    // Implementación mínima: solo imprime el formato sin formatear
    return writeStr(fd, fmt);
}