#include <stdint.h>
#include <lib.h>

#ifndef KEYBOARD_H
#define KEYBOARD_H


uint8_t getKey();

int emptyBuffer();

void keyboardHandler();

uint8_t getChar();

#endif