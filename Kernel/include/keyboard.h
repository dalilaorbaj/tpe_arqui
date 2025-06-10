#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <lib.h>

#define BUFFER_SIZE 1024
#define UNPRESSED_BIT   0x80 
#define LOWER 0
#define KEY_BITS_MASK   0x7F 
#define LSHIFT          0x2A
#define RSHIFT          0x36
#define CPS_LOCK        0x3A
#define SNAPSHOT_KEY    0x43
#define RIGHT_CONTROL   0xE01D
#define LEFT_CONTROL    0x1D
#define DELTA ('a' - 'A')
#define KEY_MAPPING_MATRIX_ROWS 2
#define KEY_MAPPING_MATRIX_COLS 128

uint64_t is_key_currently_pressed(uint8_t scancode);

uint8_t getKey();

int emptyBuffer();

void keyboardHandler();

uint8_t getChar();

extern void getSnapshot();

#endif