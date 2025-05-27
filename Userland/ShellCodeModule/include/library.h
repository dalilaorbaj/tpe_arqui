#include <stdint.h>

int getChar();

int putChar(char c);

int64_t clearScreen(void);

int64_t setFontSize(uint64_t size);

size_t strLen(const char *s);

char *numToStr(uint64_t num, uint8_t base);

int64_t puts(const char *s);

int strcmp(const char *a, const char *b);

int64_t printf(const char *fmt, ...);