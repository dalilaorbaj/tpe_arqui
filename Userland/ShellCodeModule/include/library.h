#include <stddef.h>
#include <stdarg.h>
#include <functions.h>
#include <stdint.h>
#include <syscallsHelper.h>

#define STDIN   0
#define STDOUT  1
#define STDERR  2
#define ERROR -1
#define BUFF_SIZE 8096

#define SCANCODE_W 17
#define SCANCODE_S 31  
#define SCANCODE_A 30
#define SCANCODE_D 32
#define SCANCODE_I 23
#define SCANCODE_K 37
#define SCANCODE_J 36
#define SCANCODE_L 38
#define SCANCODE_ESCAPE 1 // Tecla para salir del juego
#define SCANCODE_P 25

int64_t clearScreen(void);

char getChar();

uint64_t isKeyPressed(uint8_t scancode);

int putChar(char c);

int64_t clearScreen(void);

size_t strlen(const char *s);

char *numToStr(uint64_t num, uint8_t base);

int64_t puts(const char *s);

int strcmp(const char *a, const char *b);

int64_t writeStr(int fd, const char *s);

int64_t writeStrColor(const char *s, Color color);

void printf(char * format, ...);

char *gets(char *buf, uint16_t max_len);

int64_t beep(uint64_t frequency, uint64_t time);

float sqrtf(float number);

int64_t draw_rectangle(uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint64_t color);

int64_t draw_letter(uint64_t x, uint64_t y, uint64_t letter, uint64_t color, uint64_t size);

int64_t draw_ball(uint64_t x, uint64_t y, uint64_t radius, uint64_t color);

void flushKeyboardBuffer();