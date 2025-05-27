#include <stdint.h>
#include <stdarg.h>
#include <library.h>

#define STDIN   0
#define STDOUT  1
#define STDERR  2

int getChar(){
    uint16_t c;
    while (sys_read(STDIN, &c, 1) != 1 || c > 0xFF) {
        hlt();
    }
    return (int)(c & 0xFF);
}

int putChar(char c){
    uint8_t uc = (uint8_t)c;
    return sys_write(STDOUT, &uc, 1) == 1 ? (int)c : ERROR;
}

int64_t clearScreen(void) {
    return sys_clear_screen();
}

int64_t setFontSize(uint64_t size) {
    return sys_set_font_size(size);
}

size_t strLen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return (size_t)(p - s);
}

char *numToStr(uint64_t num, uint8_t base) {
    static char buf[65];
    char *p = buf + sizeof(buf) - 1;
    const char *digits = "0123456789abcdef";
    if (base < 2 || base > 16) base = 10;
    *p = '\0';
    do {
        *--p = digits[num % base];
        num /= base;
    } while (num);
    return p;
}

int64_t puts(const char *s) {
    int64_t written = writeStr(STDOUT, s);
    if (written >= 0) {
        putChar('\n');
        written++;
    }
    return written;
}

int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) {
        a++; b++;
    }
    return (uint8_t)*a - (uint8_t)*b;
}

int64_t printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int64_t ret = fprintf(STDOUT, fmt, args);
    va_end(args);
    return ret;
}

