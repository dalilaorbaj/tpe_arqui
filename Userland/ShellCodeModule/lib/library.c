#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <library.h>
//#include "syscallsHelper.h"

#define STDIN   0
#define STDOUT  1
#define STDERR  2

//dsp revisar si estan definidas en otros archivos
#ifndef ERROR
#define ERROR   -1
#endif
#ifndef EOF
#define EOF     -1
#endif


// int getChar(){
//     uint16_t c;
//     while (sys_read(STDIN, &c, 1) != 1 || c > 0xFF) {
//         hlt();
//     }
//     return (int)(c & 0xFF);
// }

char getChar(){
    uint16_t c;
    while(sys_read(STDIN, &c, 1)==0 || c>255);
    return (char) c;
}


// char * gets(char* buffer, uint16_t maxLen) {
//      if (!buffer || maxLen == 0) return NULL;
    
//     int current, i = 0;

//     while (1) {
//         current = getChar();
//         if(current=='\n' || current=='\r'){
//             break;
//         }
//         if ((current == '\b' || current==127) && i > 0) {
//             sys_write(STDOUT, "\b \b", 3);
//             i--;
//         } else if (current != '\b' && current != 127 && i < maxLen - 1 && current >= ' ' && current <= '~') {
//             char temp = (char)current;
//             sys_write(STDOUT, &temp, 1);
//             buffer[i++] = (char)current;
//         }
//     }
    
//     sys_write(STDOUT, "\n", 1);
//     buffer[i] = '\0';
//     return buffer;
// }

char *gets(char* buffer, uint16_t maxLen) {
    int current, i = 0;

    while (1) {
        current = getChar();
        if (current == '\n' || current == '\r') {
            break;
        }
        if (current == '\b' || current == 127) {
            if (i > 0) {
                /* retroceder un caracter en pantalla */
                putChar('\b'); putChar(' '); putChar('\b');
                i--;
            }
            continue;
        }
        if (i < maxLen - 1) {
            putChar((char)current);
            buffer[i++] = (char)current;
        }
    }
    putChar('\r'); 
    putChar('\n');  
    buffer[i] = '\0';
    return buffer;
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

size_t strlen(const char *s) {
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

