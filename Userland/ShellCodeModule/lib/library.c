#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <library.h>
//#include "syscallsHelper.h"

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
    char uc = c;
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

int64_t get_screen_info(Screen *info) {
    if (info == NULL) {
        return ERROR;
    }
    return sys_get_screen_info(info);
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
static char * convert(uint32_t num, uint32_t base){
    static char representation[] = "0123456789ABCDEF";
    static char buffer[50]; 
    char *ptr;

    ptr = &buffer[49]; 
    *ptr = '\0';

    do{
        *--ptr = representation[num % base];
        num /= base;
    }while(num != 0);

    return ptr;
}

void printf(char * format, ...) {
    va_list args;
    va_start(args, format);
    
    char *p;
    int i;
    unsigned int u;
    char *s;

    for (p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd': // signed integer
                    i = va_arg(args, int);
                    if (i < 0) {
                        putChar('-');
                        i = -i;
                    }
                    writeStr(STDOUT, convert(i, 10));
                    break;
                case 'u': // unsigned integer
                    u = va_arg(args, unsigned int);
                    writeStr(STDOUT, convert(u, 10));
                    break;
                case 'x': // hexadecimal
                    u = va_arg(args, unsigned int);
                    writeStr(STDOUT, convert(u, 16));
                    break;
                case 's': // string
                    s = va_arg(args, char *);
                    writeStr(STDOUT, s);
                    break;
                case 'c': // character
                    putChar((char)va_arg(args, int));
                    break;
                default:
                    putChar(*p);
            }
        } else {
            putChar(*p);
        }
    }

    va_end(args);
}

float sqrtf(float number) {
    if (number <= 0.0f) return 0.0f;
    float x = number;
    float y = 1.0f;
    float epsilon = 0.00001f; // Precisión deseada

    while ((x - y) > epsilon || (y - x) > epsilon) {
        x = 0.5f * (x + y);
        y = number / x;
    }
    return x;
}


int64_t draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color) {
    return sys_draw_rectangle(x, y, width, height, color);
}

int64_t draw_letter(uint64_t x, uint64_t y, uint64_t letter, uint64_t color, uint64_t size) {
    return sys_draw_letter(x, y, letter, color, size);
}

int64_t draw_ball(uint64_t x, uint64_t y, uint64_t radius, uint64_t color) {
    // Si no tienes una syscall específica para "ball", implementa como un círculo usando draw_rectangle o similar,
    // o simplemente haz un wrapper vacío por ahora para evitar el error de linker.
    // Ejemplo de stub:
    if (radius == 0) return ERROR; // No se puede dibujar una pelota de radio 0
    uint64_t diameter = radius * 2;
    return sys_draw_rectangle(x - radius, y - radius, diameter, diameter, color);
       
}
