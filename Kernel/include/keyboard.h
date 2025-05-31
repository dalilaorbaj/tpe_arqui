#include <stdint.h>
#include <lib.h>

#ifndef KEYBOARD_H
#define KEYBOARD_H


// Definir códigos especiales para las flechas
// #define KEY_UP    0x80  // 128 decimal
// #define KEY_LEFT  0x81  // 129 decimal
// #define KEY_RIGHT 0x82  // 130 decimal
// #define KEY_DOWN  0x83  // 131 decimal

#define KEY_UP    0x4800  // 128 decimal
#define KEY_LEFT  0x4B00  // 129 decimal
#define KEY_RIGHT 0x4D00  // 130 decimal
#define KEY_DOWN  0x5000  // 131 decimal

uint16_t getKey();

int emptyBuffer();

void keyboardHandler();

uint8_t getChar();

#endif