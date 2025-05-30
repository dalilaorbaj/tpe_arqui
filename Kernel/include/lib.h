#ifndef LIB_H
#define LIB_H

#include <stdint.h>
void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);

unsigned char in(unsigned short port); //devuelve el valor leido del puerto 64
void out(unsigned short port, unsigned char value);

void _hlt();
#endif