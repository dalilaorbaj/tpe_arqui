#include <stdint.h>
#include <stdarg.h>
#include "library.h"

extern void hlt(void);

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

int64_t writeStrColor(int fd, const char *s) {
    uint64_t len = 0;
    while (s[len]) len++;
    return sys_write(fd, s, len);
}



