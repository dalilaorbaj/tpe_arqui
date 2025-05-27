#ifndef KEYBOARD_H
#define KEYBOARD_H

unsigned char in(unsigned short port); //devuelve el valor leido del puerto 64
void out(unsigned short port, unsigned char value);
int hasKey();
unsigned char getKey();

#endif