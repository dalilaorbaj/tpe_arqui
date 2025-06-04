#include <stdint.h>
#include <lib.h>

#ifndef KEYBOARD_H
#define KEYBOARD_H


// Definir códigos especiales para las flechas
// #define KEY_UP    0x80  // 128 decimal
// #define KEY_LEFT  0x81  // 129 decimal
// #define KEY_RIGHT 0x82  // 130 decimal
// #define KEY_DOWN  0x83  // 131 decimal
//(?)esto no se si se usa
#define KEY_UP    0x4800  // 128 decimal
#define KEY_LEFT  0x4B00  // 129 decimal
#define KEY_RIGHT 0x4D00  // 130 decimal
#define KEY_DOWN  0x5000  // 131 decimal


int is_key_currently_pressed(uint8_t scancode);
#define SCANCODE_W 17
#define SCANCODE_S 31  
#define SCANCODE_A 30
#define SCANCODE_D 32
#define SCANCODE_I 23
#define SCANCODE_K 37
#define SCANCODE_J 36
#define SCANCODE_L 38

uint8_t getKey();

int emptyBuffer();

void keyboardHandler();

uint8_t getChar();

#endif