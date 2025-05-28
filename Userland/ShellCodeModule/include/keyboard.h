#ifndef KEYBOARD_H
#define KEYBOARD_H

#define BUFFER_SIZE 1024

#include <stdint.h>


unsigned char in(unsigned short port); //devuelve el valor leido del puerto 64
void out(unsigned short port, unsigned char value);
int hasKey();
unsigned char getKey();

uint64_t bufferHasNext();

uint64_t getCurrent();


#endif