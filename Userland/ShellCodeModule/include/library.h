#include <stddef.h>
#include <syscallsHelper.h>
#include <stdarg.h>

#define ERROR -1

int getChar();

int putChar(char c);

int64_t clearScreen(void);

int64_t setFontSize(uint64_t size);

size_t strLen(const char *s);

char *numToStr(uint64_t num, uint8_t base);

int64_t puts(const char *s);

int strcmp(const char *a, const char *b);

int64_t printf(const char *fmt, ...);

void hlt(void);

int64_t writeStr(int fd, const char *s);

int64_t fprintf(int fd, const char *fmt, va_list args);

int64_t puts(const char *s);