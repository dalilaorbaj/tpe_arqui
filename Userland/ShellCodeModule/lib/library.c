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


char getChar(){
    uint16_t c;
    while(sys_read(STDIN, &c, 1)==0 || c>255);
    return (char) c;
}

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

int64_t beep(uint64_t frequency, uint64_t time) {
    if (frequency < 20 || frequency > 20000 || time == 0) {
        return ERROR;
    }
    return sys_beep(frequency, time);
}

int64_t setFontSize(uint64_t size) {
    return sys_set_font_size(size);
}

size_t strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return (size_t)(p - s);
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


// ---------- Main lib functions ------------
// printf
// http://www.firmcodes.com/write-printf-function-c/
int64_t fprintf(uint64_t fd, const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    // int64_t out = vfprintf(fd, fmt, args);
    
    va_end(args);
    // return out;
    return 0;
}

int64_t printf(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    // int64_t out = vfprintf(STDOUT, fmt, args);
    
    va_end(args);
    // return out;
    return 0;
}



