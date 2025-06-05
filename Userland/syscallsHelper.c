#include <stdint.h>
#include <stdarg.h>
#include "library.h"

#define SCANCODE_W 17
#define SCANCODE_S 31  
#define SCANCODE_A 30
#define SCANCODE_D 32
#define SCANCODE_I 23
#define SCANCODE_K 37
#define SCANCODE_J 36
#define SCANCODE_L 38

extern int64_t sys_is_key_pressed(uint8_t scancode);

extern void hlt(void);

// Implementación de getKey (stub, deberías hacer syscall real si tu kernel la soporta)
uint8_t getKey() {
    return sys_get_key();
}

// Implementación de writeStr usando sys_write
int64_t writeStr(int fd, const char *s) {
    uint64_t len = 0;
    while (s[len]) len++;
    return sys_write(fd, s, len);
}

int64_t writeStrColor(const char *s, Color color) {
    uint64_t len = 0;
    while (s[len]) len++;
    return sys_write_color(STDOUT, s, len, color);
}
uint64_t isKeyPressed(uint8_t scancode) {
    return sys_is_key_pressed(scancode);
}

void zoomIn() {
    return sys_zoom_in();
}

void zoomOut() {
    return sys_zoom_out();
}